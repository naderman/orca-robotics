/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "mainthread.h"

using namespace std;
using namespace cameraserver;

MainThread::MainThread( const orcaice::Context &context ) :
    hydroiceutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    config_(1),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    uint32_t bpp = 3; //default bytes per pixel value
    config_.numOfCameras = (unsigned int)orcaice::getPropertyAsIntWithDefault( prop, prefix+"NumOfCameras", 1);
    std::string format = orcaice::getPropertyWithDefault( prop, prefix+"ImageFormat", "BGR8");
    if(format == "BGR8" || format == "RGB8") {
        bpp = 3;
    } else {
        bpp = 1;
    }
    
    config_.widths.resize(config_.numOfCameras);
    config_.heights.resize(config_.numOfCameras);
    config_.sizes.resize(config_.numOfCameras);
    
    for(uint32_t i = 0; i < config_.numOfCameras; ++i)
    {
        std::stringstream prefixSS;
        prefixSS << prefix << i << ".";
        config_.widths.at(i) = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixSS.str() + "ImageWidth", 320);
        config_.heights.at(i) = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixSS.str() + "ImageHeight", 240);
        config_.sizes.at(i) = bpp*config_.widths.at(i)*config_.heights.at(i);
    }

    if ( !config_.validate() ) {
        context_.tracer().error( "Failed to validate camera configuration. "+config_.toString() );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, "Failed to validate camera configuration" );
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
    orca::CameraDescriptionSequence descrs;
    
    for(unsigned int i = 0; i < descrs.size(); i++)
    {
        //transfer internal sensor configs
        
        // offset from the robot coordinate system
        orcaobj::setInit( descrs.at(i)->offset );
        descrs.at(i)->offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", descrs.at(i)->offset );
    }
    
    //
    // EXTERNAL PROVIDED INTERFACE
    //

    cameraInterface_ = new orcaifaceimpl::CameraImpl( descrs,
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
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroCameraFake.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::CameraFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::CameraFactory,DriverFactoryMakerFunc>
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

    for(unsigned int i = 0; i < orcaCameraData_.size(); ++i) 
    {
        orcaCameraData_.at(i)->timeStamp.seconds  = hydroCameraData_.timeStampSec;
        orcaCameraData_.at(i)->timeStamp.useconds = hydroCameraData_.timeStampUsec;
    }   
}

void
MainThread::walk()
{
    // Set up the camera objects
    orcaCameraData_.resize( config_.numOfCameras );

    for(unsigned int i = 0; i < orcaCameraData_.size(); ++i)
    {
        //resize image vectors
        orcaCameraData_.at(i)->data.resize( config_.sizes.at(i) );
        //set hydroCameraData pointers to be the address of orcaCameraData image vectors
        hydroCameraData_.data.at(i) = &(orcaCameraData_.at(i)->data[0]);
    }

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();

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
