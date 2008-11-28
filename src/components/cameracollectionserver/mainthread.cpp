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
using namespace cameracollectionserver;

MainThread::MainThread( const orcaice::Context &context ) 
: orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" )
, context_(context)
, config_()
, descr_(new orca::CameraCollectionDescription())
{
    subStatus().setMaxHeartbeatInterval( 20.0 );
    readSettings();
}

void
MainThread::readSettings()
{
    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    uint32_t numOfCameras = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefix + "NumberOfCameras", 1 );

    config_.resize(numOfCameras);

    for( uint32_t i = 0; i < config_.numberOfCameras; ++i )
    {
        std::stringstream prefixSS;
        prefixSS << prefix << i << ".";
        std::string prefixn;
        config_[i].width = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixn + "ImageWidth", 320);
        config_[i].height = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixn + "ImageHeight", 240);
        config_[i].format = orcaice::getPropertyWithDefault( prop, prefixn + "ImageFormat", "BGR8" );
        config_[i].size = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixn + "ImageSize", config_.width*config_.height*3);
    }

    if ( !config_.validate() ) {
        context_.tracer().error( "Failed to validate camera configuration. "+config_.toString() );
        // this will kill this component
        throw gbxutilacfr::Exception( ERROR_INFO, "Failed to validate camera configuration" );
    }
}

void
MainThread::initHardwareInterface()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroImageCollectionFake.so" );
    context_.tracer().info( "MainThread: Loading driver library "+driverLibName  );

    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::ImageCollectionFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::ImageCollectionFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        // unrecoverable error
        context_.shutdown(); 
        throw;
    }

    // create the driver
    while ( !isStopping() )
    {
        std::stringstream exceptionSS;
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset(0);
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            break;
        }
        catch ( IceUtil::Exception &e ) {
            exceptionSS << "MainThread: Caught exception while creating driver: " << e;
        }
        catch ( std::exception &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e.what();
        }
        catch ( char *e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( std::string &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( ... ) {
            exceptionSS << "MainThread: Caught unknown exception while initialising driver";
        }

        // we get here only after an exception was caught
        context_.tracer().error( exceptionSS.str() );
        subStatus().fault( exceptionSS.str() );          

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    subStatus().setMaxHeartbeatInterval( 1.0 );

}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION
    //

    
    //transfer internal sensor configs for the first camera
    descr_->width = config_[0].width;
    descr_->height = config_[0].height;
    descr_->format = config_[0].format;

    // offset from the robot coordinate system
    ifaceutil::zeroAndClear( descr_->offset );
    descr_->offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix + ".0." + "Offset", descr_->offset );

    // read size
    ifaceutil::zeroAndClear( descr_->caseSize );
    descr_->caseSize = orcaobj::getPropertyAsSize3dWithDefault( prop, prefix + ".0." + "CaseSize", descr_->caseSize );

    // frame rate
    descr_->frameRate = orcaice::getPropertyAsDoubleWithDefault( prop, prefix + ".0." + "FrameRate", 0.0 );

    //
    // Intrinsic Parameters
    //

    // Focal Length
    std::vector<double> defaultFocalLength;
    defaultFocalLength.push_back(0.0);
    defaultFocalLength.push_back(0.0);
    defaultFocalLength = orcaice::getPropertyAsDoubleVectorWithDefault( prop, prefix+"FocalLength", defaultFocalLength );
    descr_->focalLength.x =defaultFocalLength[0];
    descr_->focalLength.y =defaultFocalLength[1];

    // Principle Point
    std::vector<double> defaultPrinciplePoint;
    defaultPrinciplePoint.push_back(0.0);
    defaultPrinciplePoint.push_back(0.0);
    defaultPrinciplePoint = orcaice::getPropertyAsDoubleVectorWithDefault( prop, prefix+"PrinciplePoint", defaultPrinciplePoint );
    descr_->principlePoint.x =defaultPrinciplePoint[0];
    descr_->principlePoint.y =defaultPrinciplePoint[1];


    // Distortion Parameters
    std::vector<double> defaultDistortionParameters;
    defaultDistortionParameters.push_back(0.0);
    defaultDistortionParameters.push_back(0.0);
    defaultDistortionParameters.push_back(0.0);
    defaultDistortionParameters.push_back(0.0);
    defaultDistortionParameters = orcaice::getPropertyAsDoubleVectorWithDefault( prop, prefix+"DistortionParameters", defaultDistortionParameters );
    descr_->k1 =defaultDistortionParameters[0];
    descr_->k2 =defaultDistortionParameters[1];
    descr_->p1 =defaultDistortionParameters[0];
    descr_->p2 =defaultDistortionParameters[1];

    for(unsigned int i = 0; i < config_.extras.size(); ++i)
    {
    }

    //print out description
    context_.tracer().info( orcaobj::toString(descr_) );

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    interface_ = new orcaifaceimpl::CameraCollectionImpl( descr_
                                                    , "CameraCollection"
                                                    , context_ );
    // init
    interface_->initInterface( this, subsysName() );
}

void
MainThread::readData()
{
    //
    // Read from the image driver
    //
    hydroData_.haveWarnings = false;

    driver_->read( hydroData_ );

    orcaData_->timeStamp.seconds  = hydroData_.timeStampSec;
    orcaData_->timeStamp.useconds = hydroData_.timeStampUsec;
}

void
MainThread::walk()
{
    subStatus().initialising();
    context_.tracer().info( "Setting up Data Pointers" );
    
    // Set up the image objects
    orcaData_ = new orca::CameraData();
    orcaData_->data.resize( config_.size );
    orcaData_->description = descr_;

    // Point the pointers in hydroData_ at orcaData_
    hydroData_.data = &(orcaData_->data[0]);

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
            
            interface_->localSetAndSend( orcaData_ );
            if ( hydroData_.haveWarnings )
            {
                subStatus().warning( hydroData_.warnings );
            }
            else
            {
                subStatus().ok();
            }

            stringstream ss;
            ss << "MainThread: Read camera data: " << orcaobj::toString(orcaData_);
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
