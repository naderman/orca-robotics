/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft,
 *                         Tom Burdick
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "mainthread.h"

#include <iostream>
#include <memory>
#include <orcaice/orcaice.h>

using namespace std;
using namespace imageviewocv;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::initialise()
{
    initSettings();
    
    if ( isStopping() )
        return;

    initNetworkInterface();
}

void
MainThread::work() 
{
    setMaxHeartbeatInterval( 1.0 );

    // get 1 image
    // and check it's description for the particular image configuration
    getImage();
    
    //set up the viewer according to the configuration
    Viewer viewer = Viewer( imageData_->description->width,
                            imageData_->description->height,
                            imageData_->description->format,
                            context_ );

    while ( !isStopping() )
    {
        try 
        {
            // this blocks until a new image arrives and then
            // copies into a member variable
            getImage();
            
            // pass the image to the viewer
            viewer.display( imageData_ );
            
            //pushing too fast will cause the gui to not respond, sleep to prevent that
            // TODO: dodgy hack... needs to be fixed
            // The GUI diplay should be independent of incoming data. If the GUI dies,
            // then this code should be in the Viewer.
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(5));
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );

            // Re-create the viewer, unless we are stopping
            if ( !isStopping() ) {
                // TODO: make opencv window handle exception safe so we can create
                // another viewer here. At the moment, if an exception is thrown, 
                // the viewer will not self destruct. 
                // createViewer();
            }
        }

    } // end of while
}

void
MainThread::finalise() 
{
    context_.communicator()->shutdown();

}

////////////////////

void
MainThread::initSettings()
{
    setMaxHeartbeatInterval( 20.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    
    // int sleepIntervalMs = orcaice::getPropertyAsIntWithDefault( prop, prefix+"SleepIntervalMs", 100 );
    health().ok( "Initialized" );

}

void
MainThread::initNetworkInterface()
{
    //
    // EXTERNAL REQUIRED INTERFACE
    //
    imageInterface_ = new orcaifaceimpl::BufferedImageConsumerImpl( 1, gbxiceutilacfr::BufferTypeCircular, context_ );

    //
    // SUBSCRIBE
    //
    // try multiple times
    imageInterface_->subscribeWithTag( "Image", this, subsysName() );
}


void MainThread::getImage()
{
    //check every so often to see if we should quit
    const int timeoutMs = 1000;
    
    //TODO: should this be event based
    context_.tracer().debug("Fetching Image",6);

    // get the image from the communication buffer
    int ret = imageInterface_->buffer().getAndPopWithTimeout( imageData_, timeoutMs );
   
    if(!ret)
    {
        context_.tracer().debug("Successfuly fetched Image",6);
        // context_.tracer().debug("Adding Image to Queue",6);
                
        health().ok();
    }
    else
    {
        health().warning( "Image was not successfully copied to local buffer" );
    }

    // TODO: add string description for images
    //     stringstream ss;
    //     ss << "MainThread: Read image data: " << ifaceutil::toString(imageData_);
    //     context_.tracer().debug( ss.str(), 5 );

}

