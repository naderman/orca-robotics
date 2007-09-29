/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaimage/colourconversions.h>
#include <orcaimage/imageutils.h>

#include "mainloop.h"

using namespace std;
using namespace imageviewer;

MainLoop::MainLoop( const orca::CameraConsumerPrx & callbackPrx,
                    orcaiceutil::Buffer<orca::CameraData> & dataPipe, 
                    const orcaice::Context & context )
    : callbackPrx_(callbackPrx),
      dataPipe_(dataPipe),
      context_(context)
{
    // initialise opencv stuff
    cvNamedWindow( "ImageViewer", 1 );
}

MainLoop::~MainLoop()
{
    // delete opencv stuff
    cvReleaseImage( &cvImage_ );
    cvReleaseImage( &bayerImage_ );
}

void 
MainLoop::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // don't need to create this one, it will be cloned from the buffer
    orca::CameraData cameraData;

    // try to catch expected errors
    try
    {
    
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

    // Connect directly to the interface
    while( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::CameraPrx>( context_, cameraPrx_, "Camera" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            context_.tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }

    // get description, we need it to size the window
    while( isActive() )
    {
        try
        {
            // workaround... if imageserver and imageviewer are being run in an icebox, the
            // imageviewer needs to wait until the imageserver has loaded data
            // into the buffer... this should check rather than waiting
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    
            descr_ = cameraPrx_->getDescription();
            cout << orcaice::toString(descr_) << endl;
            break;
        }
        catch ( const orca::HardwareFailedException & e )
        {
            context_.tracer()->error( "hardware failure reported when getting a scan. Will subscribe anyway." );
        }
    }


    // setup opencv image struct for display
    // after we got the camera description!
    initCvImage();

    //
    // Subscribe for data
    //
    while ( isActive() )
    {
        try
        {
            cameraPrx_->subscribe( callbackPrx_ );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            context_.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 2000;

    //
    // This is the main loop
    //
    while( isActive() )
    {
        //
        // block on arrival of camera data
        //
        int ret = dataPipe_.getAndPopNext ( cameraData, timeoutMs );
        
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
            context_.tracer()->warning( ss.str() );
        }

    } // while
    
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // it's ok, we are probably shutting down
        cout<<"Communicator has passed away. No worries."<<endl;
    }
    
    cout<<"TRACE(mainloop.cpp): Exitting from run()" << endl;

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        stringstream ss;
        ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaiceutil::Exception & e )
    {
        stringstream ss;
        ss << "unexpected (local?) orcaice exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        stringstream ss;
        ss << "unexpected std exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
}

void 
MainLoop::initCvImage()    
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
    context_.tracer()->debug("opencv window created",5);
}
