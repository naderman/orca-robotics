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
    config_(1),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    
    config_.width = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixSS.str() + "ImageWidth", 320);
    config_.height = (uint32_t)orcaice::getPropertyAsIntWithDefault( prop, prefixSS.str() + "ImageHeight", 240);
    config_.format = orcaobj::getPropertyWithDefault( prop, prefixSS.str() + "ImageFormat", "RGB8" );

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
    
    for(unsigned int i = 0; i < config_.numOfCameras; i++)
    {
        std::stringstream prefixSS;
        prefixSS << prefix << i << ".";
        //create new descr
        orca::CameraDescriptionPtr descr(new orca::CameraDescription());
        
        //transfer internal sensor configs
        descr->imageWidth = config_.widths.at(i);
        descr->imageHeight = config_.heights.at(i);

        descr->format = (orca::ImageFormat)config_.formats.at(i);

        // offset from the robot coordinate system
        orcaobj::setInit( descr->offset );
        descr->offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefixSS.str() + "Offset", descr->offset );
       
        // read sizes
        orcaobj::setInit( descr->sensorSize );
        descr->sensorSize = orcaobj::getPropertyAsSize2dWithDefault( prop, prefixSS.str() + "SensorSize", descr->sensorSize );

        orcaobj::setInit( descr->caseSize );
        descr->caseSize = orcaobj::getPropertyAsSize3dWithDefault( prop, prefixSS.str() + "CaseSize", descr->caseSize );

        // focal length
        descr->focalLength = orcaice::getPropertyAsDoubleWithDefault( prop, prefixSS.str() + "FocalLength", 0.0 );

        //add descr to vector
        descrs_.push_back(descr);
    }
    std::cout << "Description Vector Size: " << descrs_.size() << std::endl; 

    std::cout << orcaobj::toString(descrs_) << std::endl;

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    cameraInterface_ = new orcaifaceimpl::CameraImpl( descrs_,
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
    context_.tracer().info( "MainThread: Loading driver library "+driverLibName  );
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
        catch ( ... ) {
            orcaice::catchAllExceptionsWithSleep( subStatus(), "initialising hardware driver" );
        }      
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
   

    context_.tracer().info( "Setting up Data Pointers" );
    for(unsigned int i = 0; i < config_.numOfCameras; ++i)
    {
        orcaCameraData_.push_back(orca::CameraDataPtr(new orca::CameraData()));
        //resize image vectors
        orcaCameraData_.at(i)->data.resize( config_.sizes.at(i) );
        //set hydroCameraData pointers to be the address of orcaCameraData image vectors
        hydroCameraData_.data.push_back(&(orcaCameraData_.at(i)->data[0]));
    }

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
        try 
        {
            // this blocks until new data arrives
            readData();
            
            cameraInterface_->localSetAndSend( orcaCameraData_ );
            subStatus().ok();

            stringstream ss;
            ss << "MainThread: Read camera data: " << orcaobj::toString(orcaCameraData_);
            context_.tracer().debug( ss.str(), 5 );
        } // end of try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );

            // Re-initialise the driver, unless we are stopping
            if ( !isStopping() ) {
                initHardwareDriver();
            }
        }

    } // end of while

    // Camera hardware will be shut down in the driver's destructor.
}
