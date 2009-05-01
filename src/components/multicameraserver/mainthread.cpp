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
#include <orcaobj/orcaobj.h>
#include <orcaifaceutil/camera.h>
#include <orcaimage/propertyutils.h>

#include "mainthread.h"

using namespace std;
using namespace multicameraserver;

MainThread::MainThread( const orcaice::Context &context ) 
: orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" )
, context_(context)
, descr_(new orca::MultiCameraDescription())
, config_()
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( 20.0 );
    readSettings();

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

    context_.tracer().info( "Setting up Hardware Interface" );
    initHardwareInterface();
    context_.tracer().info( "Setting up Network Interface" );
    initNetworkInterface();

    context_.tracer().info( "Setting up Data Pointers" );
 
    // Set up the image objects
    orcaData_ = new orca::MultiCameraData();

    for( unsigned int i = 0; i < descr_->descriptions.size(); ++i)
    {
        orcaData_->cameraDataVector.push_back( new orca::ImageData() );
        orcaData_->cameraDataVector[i]->pixelData.resize( descr_->descriptions[i]->size );
        orcaData_->cameraDataVector[i]->description = descr_->descriptions[i];
        hydroData_[i].pixelData = &(orcaData_->cameraDataVector[i]->pixelData[0]);
    }

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
            if ( hydroData_[0].haveWarnings )
            {
                health().warning( hydroData_[0].warnings );
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
    
    orcaimage::getMultiCameraProperties( context_, prefix, descr_ ); 

    config_.resize(descr_->descriptions.size());
    hydroData_.resize(descr_->descriptions.size());

    // copy the read in settings to the hydroimage config structures
    for( hydrointerfaces::MultiImage::Config::size_type i = 0; i < config_.size(); ++i )
    {
        orcaimage::copy( config_[i], descr_->descriptions[i] );
    }
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
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroMultiImageFake.so" );
    context_.tracer().info( "MainThread: Loading driver library "+driverLibName  );

    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::MultiImageFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::MultiImageFactory,DriverFactoryMakerFunc>
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
    for( hydrointerfaces::MultiImage::Config::size_type i = 0; i < config_.size(); ++i )
    {
        orcaimage::copy( descr_->descriptions[i], config_[i] );
    }

    setMaxHeartbeatInterval( 1.0 );

}

void
MainThread::initNetworkInterface()
{
    //print out description
    context_.tracer().info( orcaobj::toString(descr_) );

    interface_ = new orcaifaceimpl::MultiCameraImpl( descr_
                                                    , "MultiCamera"
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
    hydroData_[0].haveWarnings = false;

    driver_->read( hydroData_ );

    //copy timestamps
    for( hydrointerfaces::MultiImage::Data::size_type i = 0; i < hydroData_.size(); ++i )
    {
        orcaData_->cameraDataVector[i]->timeStamp.seconds  = hydroData_[i].timeStampSec;
        orcaData_->cameraDataVector[i]->timeStamp.useconds = hydroData_[i].timeStampUsec;
    }
}
