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
#include <orcaifacestring/image.h>
#include <orcaimageutil/imageutils.h>

#include <hydroimage/imageformats.h>


#include "mainthread.h"

using namespace std;
using namespace cameraserver;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    config_(),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    
    config_.width = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefix + "ImageWidth", 320);
    config_.height = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefix + "ImageHeight", 240);
    config_.format = orcaice::getPropertyWithDefault( prop, prefix + "ImageFormat", "BGR8" );

    if ( !config_.validate() ) {
        context_.tracer().error( "Failed to validate camera configuration. "+config_.toString() );
        // this will kill this component
        throw gbxutilacfr::Exception( ERROR_INFO, "Failed to validate camera configuration" );
    }
}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION
    //
    
    //create new descr
    orca::CameraDescriptionPtr descr_(new orca::CameraDescription());

    //transfer internal sensor configs
    descr_->width = config_.width;
    descr_->height = config_.height;

    descr_->format = config_.format;

    // offset from the robot coordinate system
    orcaobj::setInit( descr_->offset );
    descr_->offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix + "Offset", descr_->offset );

    // read size
    orcaobj::setInit( descr_->sensorSize );
    descr_->sensorSize = orcaobj::getPropertyAsSize2dWithDefault( prop, prefix + "SensorSize", descr_->sensorSize );

    orcaobj::setInit( descr_->caseSize );
    descr_->caseSize = orcaobj::getPropertyAsSize3dWithDefault( prop, prefix + "CaseSize", descr_->caseSize );

    // focal length
    descr_->focalLength = orcaice::getPropertyAsDoubleWithDefault( prop, prefix + "FocalLength", 0.0 );

    //add descr to vector
    std::cout << orcaobj::toString(descr_) << std::endl;

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    cameraInterface_ = new orcaifaceimpl::CameraImpl( descr_,
                                                              "Camera",
                                                              context_ );
    // init
    cameraInterface_->initInterface( this, subsysName() );
}

void
MainThread::initHardwareDriver()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroImageFake.so" );
    context_.tracer().info( "MainThread: Loading driver library "+driverLibName  );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::ImageFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::ImageFactory,DriverFactoryMakerFunc>
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
MainThread::readData()
{
    //
    // Read from the camera driver
    //
    hydroCameraData_.haveWarnings = false;

    driver_->read( hydroCameraData_ );

    orcaCameraData_->timeStamp.seconds  = hydroCameraData_.timeStampSec;
    orcaCameraData_->timeStamp.useconds = hydroCameraData_.timeStampUsec;
}

void
MainThread::walk()
{
   

    context_.tracer().info( "Setting up Data Pointers" );

    orcaCameraData_ = orca::CameraDataPtr(new orca::CameraData());
    //resize image vectors
    orcaCameraData_->data.resize( config_.size );
    //set hydroCameraData pointers to be the address of orcaCameraData image vectors
    hydroCameraData_.data = &(orcaCameraData_->data[0]);

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    context_.tracer().info( "Setting up Network Interface" );
    initNetworkInterface();
    context_.tracer().info( "Setting up Hardware Interface" );
    initHardwareDriver();

    context_.tracer().info( "Running..." );
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
            subStatus().ok();

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
