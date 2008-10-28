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
using namespace imageserver;

MainThread::MainThread( const orcaice::Context &context ) 
: orcaimagecommon::ImageComponentThread( context )
, orcaImageDescr_(new orca::ImageDescription())
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
    orcaImageDescr_->width = config_.width;
    orcaImageDescr_->height = config_.height;
    orcaImageDescr_->format = config_.format;

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    
    //create new interface
    imageInterface_ = new orcaifaceimpl::ImageImpl( orcaImageDescr_
                                                  , "Image"
                                                  , context_ );

    //initialize the interface
    imageInterface_->initInterface( this, subsysName() );

}

void
MainThread::readData()
{
    //
    // Read from the image driver
    //
    hydroImageData_.haveWarnings = false;
    driver_->read( hydroImageData_ );

    orcaImageData_->timeStamp.seconds  = hydroImageData_.timeStampSec;
    orcaImageData_->timeStamp.useconds = hydroImageData_.timeStampUsec;
}

void
MainThread::walk()
{
    subStatus().initialising();
    context_.tracer().info( "Setting up Data Pointers" );
    
       // These functions catch their exceptions.
    activate( context_, this, subsysName() );
    
    context_.tracer().info( "Setting up Hardware Interface" );
    initHardwareDriver();
    context_.tracer().info( "Setting up Network Interface" );
    initNetworkInterface();
    
    // Set up the image objects
    orcaImageData_ = new orca::ImageData;
    orcaImageData_->data.resize( config_.size );
    orcaImageData_->description = orcaImageDescr_;

    // Point the pointers in hydroImageData_ at orcaImageData_
    hydroImageData_.data      = &(orcaImageData_->data[0]);

    subStatus().working();
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
            
            imageInterface_->localSetAndSend( orcaImageData_ );
            if ( hydroImageData_.haveWarnings )
            {
                subStatus().warning( hydroImageData_.warnings );
            }
            else
            {
                subStatus().ok();
            }

            stringstream ss;
            ss << "MainThread: Read image data: " << orcaobj::toString(orcaImageData_);
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

    // Image hardware will be shut down in the driver's destructor.
}
