/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "mainsubsystem.h"
// #include "viewer.h"

#include <iostream>
#include <orcaice/orcaice.h>
#include <memory>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()

using namespace std;
using namespace imageview;

MainSubsystem::MainSubsystem( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainSubsystem" ),
    context_(context)
{
}

void
MainSubsystem::initialise()
{
    initSettings();
    
    createViewer();
    
    if ( isStopping() )
        return;

    initNetworkInterface();
}

void
MainSubsystem::work() 
{
    subStatus().setMaxHeartbeatInterval( 1.0 );

    while ( !isStopping() )
    {
        try 
        {
            // this blocks until a new image arrives and then
            // copies into a member variable
            getImage();
            
            // pass the image to the viewer
            viewer_->display( imageData_ );
            
            //pushing too fast will cause the gui to not respond, sleep to prevent that
            // TODO: dodgy hack... needs to be fixed
            // The GUI diplay should be independent of incoming data. If the GUI dies,
            // then this code should be in the Viewer.
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(5));
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );

            // Re-create the viewer, unless we are stopping
            if ( !isStopping() ) {
                createViewer();
            }
        }

    } // end of while
}

void
MainSubsystem::finalise() 
{
    delete viewer_;
    context_.communicator()->shutdown();

}

////////////////////

void
MainSubsystem::initSettings()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    
    int sleepIntervalMs = orcaice::getPropertyAsIntWithDefault( prop, prefix+"SleepIntervalMs", 100 );
    subStatus().ok( "Initialized" );

//TODO: add this in. We should be checking configuration of the Viewer here.
//     if ( !config_.validate() ) {
//         context_.tracer().error( "Failed to validate laser configuration. "+config_.toString() );
//         // this will kill this component
//         throw gbxutilacfr::Exception( ERROR_INFO, "Failed to validate laser configuration" );
//     }


//     // Set up the laser-scan objects
//     orcaLaserData_ = new orca::LaserScanner2dData;
//     orcaLaserData_->minRange     = config_.minRange;
//     orcaLaserData_->maxRange     = config_.maxRange;
//     orcaLaserData_->fieldOfView  = config_.fieldOfView;
//     orcaLaserData_->startAngle   = config_.startAngle;
//     orcaLaserData_->ranges.resize( config_.numberOfSamples );
//     orcaLaserData_->intensities.resize( config_.numberOfSamples );
// 
//     // Point the pointers in hydroLaserData_ at orcaLaserData_
//     hydroLaserData_.ranges      = &(orcaLaserData_->ranges[0]);
//     hydroLaserData_.intensities = &(orcaLaserData_->intensities[0]);
}

void
MainSubsystem::initNetworkInterface()
{
    // activate component's communication
    activate( context_, this, subsysName() );
    
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

void
MainSubsystem::createViewer()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // The plugin viewer we want to use
    std::string viewerType = 
        orcaice::getPropertyWithDefault( prop, prefix+"ViewerType", "QtViewer" );
    context_.tracer().debug( "MainSubsystem: Loading "+viewerType, 4 );
    
    try 
    {
        // create the actual viewer
        viewer_ = Viewer::factory( viewerType, context_ );
        
        // initialise parameters for viewer
        viewer_->initialise();
    }
    catch (Viewer::BadViewerCreation &e)
    {
        // unrecoverable error
        context_.shutdown(); 
        throw;
    }
}

void MainSubsystem::getImage()
{
    //check every so often to see if we should quit
    const int timeoutMs = 1000;
    
    //TODO: should this be event based
    context_.tracer().debug("Fetching Image",6);

    // get the image from the communication buffer
    int ret = imageInterface_->buffer().getAndPopNext( imageData_, timeoutMs );
   
    if(!ret)
    {
        context_.tracer().debug("Successfuly fetched Image",6);
        // context_.tracer().debug("Adding Image to Queue",6);
        
        // send the image to a local buffer which the Viewer can access
        // at its leisure
        // imageQueue_.push( imageData_ );
        
        subStatus().ok();
    }
    else
    {
        subStatus().warning( "Image was not successfully copied to local buffer" );
    }


    // TODO: add string description for images
    //     stringstream ss;
    //     ss << "MainSubsystem: Read image data: " << ifaceutil::toString(imageData_);
    //     context_.tracer().debug( ss.str(), 5 );

}
