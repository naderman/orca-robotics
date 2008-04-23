/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaimage/colourconversions.h>
#include <orcaimage/imageutils.h>
#include <orcaifaceimpl/bufferedconsumers.h>

#include "mainthread.h"

using namespace std;
using namespace cameraviewer;

MainThread::MainThread( const orcaice::Context& context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
    // initialise opencv stuff
    //its a bit funny here, I just assume max 3 cameras for now
    cvNamedWindow( "CameraViewer1", 1 );
    cvNamedWindow( "CameraViewer2", 1 );
    cvNamedWindow( "CameraViewer3", 1 );
}

MainThread::~MainThread()
{
    // delete opencv stuff
    for(int i = 0; i < images_.size(); ++i)
    {
        cvReleaseImage( &images_[i] );
    }
}

void 
MainThread::walk()
{    
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // don't need to create this one, it will be cloned from the buffer
    orca::CameraDataSequence imageData;
    
    // count the number of images received
    int numImages = 0;
    orca::Time arrivalTime;
    orca::Time initialImageTime;
    orca::Time finalImageTime;
    double diff = 0.0;
    double avDiff = 0.0;
    double totalTime;

    //
    // REQUIRED INTERFACE: Image 
    //
    //           
    // Connect directly to the interface
    orcaice::connectToInterfaceWithTag<orca::CameraPrx>( context_, cameraPrx_, "Camera", this, subsysName() );

    // get description, we need it to size the window
    while( !isStopping() )
    {
        try
        {
            descr_ = cameraPrx_->getDescription();
            cout << "Description Vector Size: " << descr_.size() << endl;
            cout << orcaobj::toString(descr_) << endl;
            break;
        }
        catch ( const orca::HardwareFailedException & e )
        {
            context_.tracer().warning( "hardware failure reported when getting a signle image. Will try again." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }
    }

    // setup opencv image struct for display
    // after we got the camera description!
    initCvImage();

    //
    // Subscribe for data
    //
    orcaifaceimpl::BufferedCameraConsumerImplPtr cameraInterface =
        new orcaifaceimpl::BufferedCameraConsumerImpl( 10, gbxsickacfr::gbxiceutilacfr::BufferTypeCircular, context_);
    // multi-try function
    cameraInterface->subscribeWithTag( "Camera", this, subsysName() );
    
    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 500;

    //
    // This is the main loop
    //
    while( !isStopping() )
    {
        //
        // block on arrival of camera data
        //
        int ret = cameraInterface->buffer().getAndPopNext ( imageData, timeoutMs );
        
        if ( !ret ) 
        {
            //*********************************
            // Timing performance
            // TODO: Put this into a separate function       
            //*********************************       
            // delay checks for sending objects
            int averageOver = 100;
            orcaice::setToNow( arrivalTime );
            diff += abs(orcaice::timeDiffAsDouble( imageData.at(0)->timeStamp, arrivalTime ));
            
            // throughput check
            if ((int)diff == 0)
            {
                orcaice::setToNow( initialImageTime );
            }              
            
            if ( fmod((double)numImages++,(double)averageOver) == 0 )
            {
                // calculate average delay
                avDiff = diff/averageOver;
                cout << "TRACE(imagehandler.cpp): Average delay of images between grabbing and viewing: " << avDiff << "sec" << endl;
                diff = 0.0;
                
                // calculate throughput in Hz
                orcaice::setToNow( finalImageTime );
                totalTime = orcaice::timeDiffAsDouble( finalImageTime, initialImageTime );
                cout << "TRACE(imagehandler.cpp): Images are arriving at " << 1/(totalTime/averageOver) << " Hz." << endl << endl;
            }
            //end of timing performance **********************************
        }

        if (ret == 0)
        {
            cout << "INFO(mainloop.cpp): Getting imageData from buffer... " << endl;

            //
            // execute algorithm to display image
            //
            
            // make sure the image is in BGR format which opencv can display       
            orcaimage::cvtToBgr( images_.at(0), imageData.at(0), descr_.at(0) );
            // load the image into the previously created window
            cvShowImage( "CameraViewer1", images_.at(0) );
            
            if(imageData.size() >= 2 ) {
                orcaimage::cvtToBgr( images_.at(1), imageData.at(1), descr_.at(1) );
                cvShowImage( "CameraViewer2", images_.at(1) );
            }

            if(imageData.size() >= 3 ) {
                orcaimage::cvtToBgr( images_.at(2), imageData.at(2), descr_.at(2) );
                cvShowImage( "CameraViewer3", images_.at(2));
            }            
            // need this as opencv doesn't display properly otherwise
            cvWaitKey(10);

        }
        else {
            stringstream ss;
            ss << "Timed out (" << timeoutMs << "ms) waiting for camera data.";
            context_.tracer().warning( ss.str() );
        }

    } // while
}

void 
MainThread::initCvImage()    
{
    cout << "TRACE(imagehandler.cpp):Image Format - " << orcaobj::toString( descr_.at(0)->format ) << endl;
    
    // should this be done at the imageserver level and depend on the mode and format?
    // maybe nChannels should be in the Camera object
    // TODO: put this nChannel calculation into imageutils as a separate function 
    
    // opencv gear here
    for(int i = 0; i < descr_.size(); ++i)
    {
        images_.push_back(cvCreateImage( cvSize( descr_.at(i)->imageWidth, descr_.at(i)->imageHeight ),  8, 3 ));
    }

    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    context_.tracer().debug("opencv window created",5);
}
