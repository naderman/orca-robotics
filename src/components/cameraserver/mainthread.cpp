/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaifaceutil/camera.h>

#include "mainthread.h"

using namespace std;
using namespace cameraserver;

MainThread::MainThread( const orcaice::Context &context ) 
: orcaimagecommon::ImageComponentThread( context )
, orcaCameraDescr_(new orca::CameraDescription())
{
}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION
    //

    //transfer internal sensor configs
    orcaCameraDescr_->width = config_.width;
    orcaCameraDescr_->height = config_.height;
    orcaCameraDescr_->format = config_.format;

    // offset from the robot coordinate system
    ifaceutil::zeroAndClear( orcaCameraDescr_->offset );
    orcaCameraDescr_->offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix + "Offset", orcaCameraDescr_->offset );

    // read size
    ifaceutil::zeroAndClear( orcaCameraDescr_->sensorSize );
    orcaCameraDescr_->sensorSize = orcaobj::getPropertyAsSize2dWithDefault( prop, prefix + "SensorSize", orcaCameraDescr_->sensorSize );

    ifaceutil::zeroAndClear( orcaCameraDescr_->caseSize );
    orcaCameraDescr_->caseSize = orcaobj::getPropertyAsSize3dWithDefault( prop, prefix + "CaseSize", orcaCameraDescr_->caseSize );

    // focal length
    orcaCameraDescr_->focalLength = orcaice::getPropertyAsDoubleWithDefault( prop, prefix + "FocalLength", 0.0 );

    //add descr to vector
    std::cout << orcaobj::toString(orcaCameraDescr_) << std::endl;

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    cameraInterface_ = new orcaifaceimpl::CameraImpl( orcaCameraDescr_
                                                    , "Camera"
                                                    , context_ );
    // init
    cameraInterface_->initInterface( this, subsysName() );
}

void
MainThread::readData()
{
    //
    // Read from the image driver
    //
    hydroImageData_.haveWarnings = false;

    driver_->read( hydroImageData_ );

    orcaCameraData_->timeStamp.seconds  = hydroImageData_.timeStampSec;
    orcaCameraData_->timeStamp.useconds = hydroImageData_.timeStampUsec;
}

void
MainThread::walk()
{
    subStatus().initialising();
    context_.tracer().info( "Setting up Data Pointers" );
    
    // Set up the image objects
    orcaCameraData_ = new orca::CameraData();
    orcaCameraData_->data.resize( config_.size );
    orcaCameraData_->description = orcaCameraDescr_;

    // Point the pointers in hydroImageData_ at orcaCameraData_
    hydroImageData_.data = &(orcaCameraData_->data[0]);

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    context_.tracer().info( "Setting up Network Interface" );
    initNetworkInterface();
    context_.tracer().info( "Setting up Hardware Interface" );
    initHardwareDriver();

    context_.tracer().info( "Running..." );
    subStatus().working();    

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( !isStopping() )
    {
        stringstream exceptionSS;
        try 
        {
            // this blocks until new data arrives
            readData();
            
            cameraInterface_->localSetAndSend( orcaCameraData_ );
            if ( hydroImageData_.haveWarnings )
            {
                subStatus().warning( hydroImageData_.warnings );
            }
            else
            {
                subStatus().ok();
            }

            stringstream ss;
            ss << "MainThread: Read camera data: " << orcaobj::toString(orcaCameraData_);
            context_.tracer().debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & ) {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e;
        }
        catch ( const std::exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e.what();
        }
        catch ( const std::string &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected string: " << e;
        }
        catch ( const char *e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected char *: " << e;
        }
        catch ( ... ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected unknown exception.";
        }

        if ( !exceptionSS.str().empty() ) {
            context_.tracer().error( exceptionSS.str() );
            subStatus().fault( exceptionSS.str() );     
            // Slow things down in case of persistent error
            sleep(1);
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initHardwareDriver();

    } // end of while

    // Camera hardware will be shut down in the driver's destructor.
}
