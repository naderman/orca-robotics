/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "imagehandler.h"
#include "imageutil.h"

#include <iostream>
#include <orcaice/orcaice.h>


using namespace std;
using namespace orca;
using namespace imageviewer;
using orcaice::operator<<;

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
    //CvDisplayer cvDisplayer();
    cvNamedWindow( "ImageViewer", 1 );
}

ImageHandler::~ImageHandler()
{
    // delete opencv stuff
    cvReleaseImage( &cvImage_ );
}

void ImageHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // don't need to create this one, it will be cloned from the buffer
    orca::CameraDataPtr cameraData;

    // opencv gear here
    cvImage_ = cvCreateImage( cvSize( cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight ),  8, 3 );
    //context_.tracer()->debug("opencv window created",5);

    // dodgy opencv has a bug which requires this...
    cvResizeWindow( "ImageViewer", cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight );
    cvWaitKey(15);
    cvResizeWindow( "ImageViewer", cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight );
    cvWaitKey(15);
    cvResizeWindow( "ImageViewer", cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight );
    cvWaitKey(15);
    cvResizeWindow( "ImageViewer", cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight );
    cvWaitKey(15);
    cvResizeWindow( "ImageViewer", cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight );
    cvWaitKey(15);
    cvResizeWindow( "ImageViewer", cameraPrx_->getData()->imageWidth, cameraPrx_->getData()->imageHeight );
    cvWaitKey(15);


    // try to catch expected errors
    try
    {
       
    // get camera config and geometry (only once)
    cout << "Getting config and geometry in imagehandler.cpp as a test" << endl;
    cameraConfigPtr_ = cameraPrx_->getConfig();
    cameraGeometryPtr_ = cameraPrx_->getGeometry();

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
        int ret = cameraDataBuffer_.getAndPopNext ( cameraData, timeoutMs );
        
        if (ret == 0)
        {
            cout << "INFO(mainloop.cpp): Getting cameraData from buffer... " << endl;

            //
            // execute algorithm to display image
            //
            cout << "incoming image size: " << cameraData->image.size() << endl;
            bool bayer = false;
            if ( bayer )
            {
                // colourFrame if bayer
                IplImage* frame  = cvCreateImage( cvSize( 1024, 768),  8, 1 );

                memcpy( frame->imageData, &cameraData->image[0], cameraData->image.size() );
                cvCvtColor( frame, cvImage_, CV_BayerGR2BGR );
            }
            else
            {
                // copy the data from the camera object into the opencv structure
                memcpy( cvImage_->imageData, &cameraData->image[0], cameraData->image.size() );
            }
            // convert to opencv format BGR (why the hell do they store it back to front?)
            //imageviewer::cvtToBgr(cvImage_->imageData, cameraData);

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
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
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

