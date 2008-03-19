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
using namespace imageviewer;

MainThread::MainThread( const orcaice::Context& context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
    // initialise opencv stuff
    cvNamedWindow( "ImageViewer", 1 );
}

MainThread::~MainThread()
{
    // delete opencv stuff
    cvReleaseImage( &cvImage_ );
    cvReleaseImage( &bayerImage_ );
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
    orca::CameraData cameraData;
    
    // count the number of images received
    int numImages = 0;
    orca::Time arrivalTime;
    orca::Time initialImageTime;
    orca::Time finalImageTime;
    double diff = 0.0;
    double avDiff = 0.0;
    double totalTime;

    //
    // REQUIRED INTERFACE: Camera
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
        int ret = cameraInterface->buffer().getAndPopNext ( cameraData, timeoutMs );
        
        if ( !ret ) 
        {
            //*********************************
            // Timing performance
            // TODO: Put this into a separate function       
            //*********************************       
            // delay checks for sending objects
            int averageOver = 100;
            orcaice::setToNow( arrivalTime );
            diff += orcaice::timeDiffAsDouble( cameraData.timeStamp, arrivalTime );
            
            // throughput check
            if ((int)diff == 0)
            {
                orcaice::setToNow( initialImageTime );
            }              
            
            if ( fmod((double)numImages++,(double)averageOver) == 0 )
            {
                // calculate average delay
                avDiff = diff/averageOver;
                // cout << "TRACE(imagehandler.cpp): Average delay of images between grabbing and viewing: " << avDiff << "sec" << endl;
                diff = 0.0;
                
                // calculate throughput in Hz
                orcaice::setToNow( finalImageTime );
                totalTime = orcaice::timeDiffAsDouble( finalImageTime, initialImageTime );
                // cout << "TRACE(imagehandler.cpp): Images are arriving at " << 1/(totalTime/averageOver) << " Hz." << endl << endl;
            }
            //end of timing performance **********************************
        }

        if (ret == 0)
        {
            cout << "INFO(mainloop.cpp): Getting cameraData from buffer... " << endl;

            //
            // execute algorithm to display image
            //
            
            // make sure the image is in BGR format which opencv can display       
            orcaimage::cvtToBgr( cvImage_, bayerImage_, cameraData );
            // load the image into the previously created window
            cvShowImage( "ImageViewer", cvImage_ );
            // need this as opencv doesn't display properly otherwise
            cvWaitKey(15);

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
    cout << "TRACE(imagehandler.cpp):Image Format - " << orcaimage::formatName( descr_.format ) << endl;
    
    // should this be done at the imageserver level and depend on the mode and format?
    // maybe nChannels should be in the Camera object
    // TODO: put this nChannel calculation into imageutils as a separate function 
    
    // default number of channels for a colour image
    int nChannels = 3;
    int nBayerChannels = 1;   
    if( descr_.format == orca::ImageFormatBayerBg  || descr_.format == orca::ImageFormatBayerGb 
            || descr_.format == orca::ImageFormatBayerRg || descr_.format == orca::ImageFormatBayerGr )
    {
        // set up an IplImage struct for the Greyscale bayer encoded data
        bayerImage_  = cvCreateImage( cvSize( descr_.imageWidth, descr_.imageHeight ),  8, nBayerChannels );
        cout << "Image is Bayer encoded: " << endl;
        // cout << "bayer encoding: " << format << endl;
    }
    else if ( descr_.format == orca::ImageFormatModeGray )
    {
        // display image is greyscale therefore only 1 channel      
        nChannels = 1;
    }      

    // opencv gear here
    cvImage_ = cvCreateImage( cvSize( descr_.imageWidth, descr_.imageHeight ),  8, nChannels );
    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    context_.tracer().debug("opencv window created",5);
}
