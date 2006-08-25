/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "imagehandler.h"

#include <iostream>
#include <orcaice/orcaice.h>

#include <orcaimage/colourconversions.h>
#include <orcaimage/imageutils.h>

using namespace std;
using namespace orca;
using namespace imageviewer;

ImageHandler::ImageHandler( CameraPrx cameraPrx,
                            orcaice::PtrBuffer<CameraDataPtr> &cameraDataBuffer, 
                            const orcaice::Context & context )
    : cameraPrx_(cameraPrx),
      cameraDataBuffer_(cameraDataBuffer),
      context_(context)
{
    // this is the last place we can throw exceptions from.
    // after this the thread will be launched
    init();
}

void ImageHandler::init()
{
    // initialise opencv stuff
    cvNamedWindow( "ImageViewer", 1 );
}

ImageHandler::~ImageHandler()
{
    // delete opencv stuff
    cvReleaseImage( &cvImage_ );
    cvReleaseImage( &bayerImage_ );
}

void ImageHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // don't need to create this one, it will be cloned from the buffer
    orca::CameraDataPtr cameraData;

    // try to catch expected errors
    try
    {
       
    // get camera config and geometry (only once)
    cout << "Getting config and geometry in imagehandler.cpp as a test" << endl;
    cameraConfigPtr_ = cameraPrx_->getConfig();
    cameraGeometryPtr_ = cameraPrx_->getGeometry();

    // setup opencv image struct for display
    ImageHandler::initCvImage();
    
    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 2000;
    
    // count the number of images received
    int numImages = 0;
    orca::Time arrivalTime;
    orca::Time initialImageTime;
    orca::Time finalImageTime;
    double diff = 0.0;
    double avDiff = 0.0;
    double totalTime;
    
    //
    // This is the main loop
    //
    while( isActive() )
    {
        //
        // block on arrival of camera data
        //
        int ret = cameraDataBuffer_.getAndPopNext ( cameraData, timeoutMs );
        
        //*********************************
        // Timing performance
        // TODO: Put this into a separate function       
        //*********************************       
        // delay checks for sending objects
        int averageOver = 100;
        orcaice::setToNow( arrivalTime );
        diff += orcaice::timeDiffAsDouble( cameraData->timeStamp, arrivalTime );
        
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
    catch ( const orcaice::Exception & e )
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

void ImageHandler::initCvImage()    
{
    ImageFormat format = cameraPrx_->getConfig()->format;
    cout << "TRACE(imagehandler.cpp):Image Format - " << orcaimage::formatName( format ) << endl;
    
    // should this be done at the imageserver level and depend on the mode and format?
    // maybe nChannels should be in the Camera object
    // TODO: put this nChannel calculation into imageutils as a separate function 
    int nChannels;
    if( format == BAYERBG | format == BAYERGB | format == BAYERRG | format == BAYERGR )
    {
        nChannels = 1;
        // set up an IplImage struct for the Greyscale bayer encoded data
        bayerImage_  = cvCreateImage( cvSize( cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight ),  8, nChannels );
        cout << "Image is Bayer encoded: " << endl;
        // cout << "bayer encoding: " << format << endl;
    }
    else
    {
        nChannels = 3;
    }

    // opencv gear here
    cvImage_ = cvCreateImage( cvSize( cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight ),  8, 3 );
    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    context_.tracer()->debug("opencv window created",5);

}
