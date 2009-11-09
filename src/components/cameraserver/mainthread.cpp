/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/image.h>
#include <orcaobj/camera.h>
#include <orcaifaceutil/camera.h>
#include <orcaimage/propertyutils.h>

#include "mainthread.h"

using namespace std;
using namespace cameraserver;

MainThread::MainThread( const orcaice::Context &context ) 
: orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" )
, context_(context)
, descr_(new orca::CameraDescription())
, config_()
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( 20.0 );
    readSettings();
    // print out description
    context_.tracer().debug( orcaobj::toString(descr_) );

    context_.tracer().info( "Setting up Hardware Interface" );
    initHardwareInterface();
    
    // print out description
    context_.tracer().debug( orcaobj::toString(descr_) );

    context_.tracer().info( "Setting up Network Interface" );
    initNetworkInterface();


    context_.tracer().info( "Setting up Data Pointers" );
    // Set up the image objects
    orcaData_ = new orca::ImageData();
    orcaData_->pixelData.resize( descr_->size );
    orcaData_->description = descr_;

    // Point the pointers in hydroData_ at orcaData_
    hydroData_.pixelData = &(orcaData_->pixelData[0]);
}

void
MainThread::work()
{
    while ( !isStopping() )
    {
        try 
        {
            // this blocks until new data arrives
            readData();
            
            interface_->localSetAndSend( orcaData_ );
            if ( hydroData_.haveWarnings )
            {
                health().warning( hydroData_.warnings );
            }
            else
            {
                health().ok();
            }

            stringstream ss;
            ss << "MainThread: Read camera data: " << orcaobj::toString(orcaData_);
            context_.tracer().debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );

            // Re-initialise the driver, unless we are stopping
            if ( !isStopping() ) {
                initHardwareInterface();
            }
        }
    } // end of while

    // Camera hardware will be shut down in the driver's destructor.
}

////////////////////

void
MainThread::readSettings()
{
    std::string prefix = context_.tag() + ".Config.";
    orcaimage::getCameraProperties( context_, prefix, descr_ ); 
    
    // copy the read in settings to the hydroimage config structures
    orcaimage::copy( config_, descr_);
}

void
MainThread::initHardwareInterface()
{
    setMaxHeartbeatInterval( 20.0 );

    //copy from description to config

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
            hydrodll::dynamicallyLoadClass<hydrointerfaces::ImageFactory,ImageDriverFactoryMakerFunc>
            ( *driverLib_, "createCameraServerDriverFactory" ) );
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
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset(0);
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "initialising hardware driver", health() );
        }         
    }

    //copy from config to description for the possible changes made
    orcaimage::copy( descr_, config_ );

    setMaxHeartbeatInterval( 1.0 );

}

void
MainThread::initNetworkInterface()
{
        interface_ = new orcaifaceimpl::CameraImpl( descr_
                                                    , "Camera"
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
