/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "viewer.h" 
#include "colourconversions.h"

#include <hydroimage/formats.h>
// for calculating fps
#include <orcaice/timeutils.h>

using namespace multicameraviewocv;
using namespace hydroimage;

Viewer::Viewer( const orca::MultiCameraDataPtr& multiCameraData,
                const orcaice::Context& context ) :
    context_(context)
{

    // Assume each camera is identical and the image formats are identical
    std::string format = multiCameraData->cameraDataVector.at(0)->description->format;
    int width = multiCameraData->cameraDataVector.at(0)->description->width;
    int height = multiCameraData->cameraDataVector.at(0)->description->height;
    
    // class to search for image format properties
    ImageFormat imageFormat = ImageFormat::find( format );
    
    // set up opencv storage for the source image
    int depth = imageFormat.getBitsPerPixel()/imageFormat.getNumberOfChannels();
    cvSrcImage_ = cvCreateImage( cvSize( width, height ),  depth, imageFormat.getNumberOfChannels() );
    
    // set up opencv storage for the display image
    std::string displayFormat = "BGR8";
    ImageFormat imageDisplayFormat = ImageFormat::find( displayFormat );
    depth = imageDisplayFormat.getBitsPerPixel()/imageDisplayFormat.getNumberOfChannels();
    cvDisplayImage_ = cvCreateImage( cvSize( width, height ),  depth, imageDisplayFormat.getNumberOfChannels() );

    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    
    name_ = "MultiCameraViewer";
    cvNamedWindow( name_ );
    // context_.tracer()->debug("opencv window created",5);
    
    // start the timer for calculating the number of frames per second
    // the images are being displayed at
    orcaice::setToNow( oldFrameTime_ );
    
    // initialise font for displaying fps
    cvInitFont(&font_, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0.0, 1, CV_AA);
        
}

Viewer::~Viewer()
{
    cvDestroyWindow( name_ );
    cvReleaseImage( &cvSrcImage_ );
    cvReleaseImage( &cvDisplayImage_ );
}

void Viewer::display( orca::MultiCameraDataPtr& images )
{
    // check the user hasn't closed the window
    if( cvGetWindowHandle( name_ ) != 0 )
    {
        // Point the opencv image data structure to the orca image data
        // Don't want to memcpy to reduce the total copies that occur
        // memcpy( cvImage_->imageData, &(image->data[0]), image->data.size() );
        cvSrcImage_->imageData = (char*)(&images->cameraDataVector.at(0)->data[0]);
            
        // Convert the image format to BGR8 for viewing in an opencv window.
        cvtToBgr( cvSrcImage_, cvDisplayImage_, images->cameraDataVector.at(0)->description->format );
            
        displayFrameRate();
            
        // display the image in an opencv window
        cvShowImage( name_, cvDisplayImage_ );
            
        // Without this, the window doesn't display propery... what the hell?
        cvWaitKey(5);
    }
    else
    {
        // no window to display in so shutdown the component
        context_.communicator()->shutdown();
    }
}
    
void
Viewer::displayFrameRate()
{
        // Display the frame rate
        orcaice::setToNow( currentFrameTime_ );
        diff_  = orcaice::timeDiffAsDouble( currentFrameTime_, oldFrameTime_ );
        oldFrameTime_ = currentFrameTime_;
        fps_ = 1.0/diff_;
        
        std::stringstream fpsString_;
        fpsString_ << "fps = " << fps_;
        cvPutText(cvDisplayImage_, fpsString_.str().c_str(), cvPoint(20,20), &font_, CV_RGB(255,255,255)); 
}
    
