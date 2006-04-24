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
// #include "imageutil.h"

#include <iostream>
#include <orcaice/orcaice.h>

#include <triclops/triclops.h>
#include <triclops/pnmutils.h> 

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


    // try to catch expected errors
    try
    {
       
    // get camera config and geometry (only once)
    cout << "Getting config and geometry in imagehandler.cpp as a test" << endl;
    cameraConfigPtr_ = cameraPrx_->getConfig();
    cameraGeometryPtr_ = cameraPrx_->getGeometry();

    ImageFormat format = cameraPrx_->getConfig()->format;

    // should this be done at the imageserver level and depend on the mode and format?
    // maybe nChannels should be in the Camera object 
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

    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 2000;
    
    // count the number of images received
    int numImages = 0;
    orca::Time arrivalTime;
    double diff = 0.0;
    double avDiff = 0.0;

    //
    // This is the main loop
    //
    while( isActive() )
    {
        //
        // block on arrival of camera data
        //
        int ret = cameraDataBuffer_.getAndPopNext ( cameraData, timeoutMs );
        
        // time checks
        int averageOver = 160;
        orcaice::setToNow( arrivalTime );
        diff += orcaice::timeDiffAsDouble( cameraData->timeStamp, arrivalTime );
        if ( fmod((double)numImages++,(double)averageOver) == 0 )
        {
            avDiff = diff/averageOver;
            cout << "             avDiff: " << avDiff << " \n\n\n" << endl;
            diff = 0.0;
        }

        if (ret == 0)
        {
            cout << "INFO(mainloop.cpp): Getting cameraData from buffer... " << endl;

            //
            // execute algorithm to display image
            //
            
            if ( cameraData->format == BAYERBG | cameraData->format == BAYERGB | cameraData->format == BAYERRG | cameraData->format == BAYERGR )
            {
                // check if the image is bayer encoded

                // copy the data from the camera object into the opencv structure
                memcpy( bayerImage_->imageData, &cameraData->image[0], cameraData->image.size() );
                // decode and convert to colour
                if( cameraData->format == BAYERBG )
                {
                    cvCvtColor( bayerImage_, cvImage_, CV_BayerBG2BGR );
                }
                else if( cameraData->format == BAYERGB )
                {
                    cvCvtColor( bayerImage_, cvImage_, CV_BayerGB2BGR );
                }
                else if( cameraData->format == BAYERRG )
                {
                    cvCvtColor( bayerImage_, cvImage_, CV_BayerRG2BGR );
                }
                else if( cameraData->format == BAYERGR )
                {
                    cvCvtColor( bayerImage_, cvImage_, CV_BayerGR2BGR );
                }
            }
            else if( format == TRICLOPSRGB)
            {
                // TODO: might be able to use the pgr stuff from triclops
                // For now just load into an IplImage struct
                TriclopsInput triclopsInput;
                // TODO: this shouldn't be hardcoded... depends on the image properties
                triclopsInput.nrows = 768;
                triclopsInput.ncols = 1024;
                triclopsInput.inputType = TriInp_RGB;
                int imageSize = triclopsInput.nrows * triclopsInput.ncols;

                // set up some temporary storage buffers
                // TODO: do we need to do this?
                vector<char> red;
                vector<char> green;
                vector<char> blue;
                red.resize(imageSize);
                green.resize(imageSize);
                blue.resize(imageSize);

                // the R,G, and B values are stored in separate arrays
                memcpy(&red[0], &cameraData->image[0], imageSize );
                memcpy(&green[0], &cameraData->image[0+imageSize], imageSize );
                memcpy(&blue[0], &cameraData->image[0+2*imageSize], imageSize );

                // TODO: can we copy directly into the triclopsInput?
                triclopsInput.u.rgb.red = &red[0];
                triclopsInput.u.rgb.green = &green[0];
                triclopsInput.u.rgb.blue = &blue[0];

//                 char file[] = "test2.ppm";
//                 bool res = ppmWriteFromTriclopsInput( file , &triclopsInput );
//                 cout << "res: " << res << endl;

                // Triclops Input stores the data from each of the
                // images in 3 separate buffers. Triclops libraries
                // view the data as if each of the bayer encoded images (B&W) was a colour
                // channel. eg. the top camera is the blue channel,
                // left camera=red channel, etc. However opencv
                // requires the rgb data to be interleaved:
                for( int i = 0; i < 3*cameraData->imageHeight*cameraData->imageWidth; i += 3 )
                {
                     cvImage_->imageData[i] = red[i/3];
                     cvImage_->imageData[i+1] = green[i/3];
                     cvImage_->imageData[i+2] = blue[i/3];
                }
            }

            else
            {
                // no bayer encoding

                // copy the data from the camera object into the opencv structure
                memcpy( cvImage_->imageData, &cameraData->image[0], cameraData->image.size() );
            }

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

