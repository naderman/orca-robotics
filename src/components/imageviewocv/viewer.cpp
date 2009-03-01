/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "viewer.h" 
#include <hydroimage/formats.h>
#include <hydroimageocvutil/colourconversions.h>

// for calculating fps
#include <orcaice/timeutils.h>

using namespace imageviewocv;
using namespace hydroimage;
using namespace hydroimageocvutil;

Viewer::Viewer( const int width,
                const int height,
                const std::string& format,
                const orcaice::Context& context ) :
    context_(context)
{
    isPadded_ = false;
    
    // class to search for image format properties
    ImageFormat imageSrcFormat = ImageFormat::find( format );
    int numChannels = imageSrcFormat.getNumberOfChannels();
    int depth = imageSrcFormat.getBitsPerPixel()/numChannels;
    
    // set up opencv storage for the source image
    cvSrcImage_ = cvCreateImage( cvSize( width, height ),  depth, numChannels );
    
    // check if opencv has padded the byte array so that the width is a multiple of 4 or 8 bytes
    orcaByteWidth_ = width*numChannels;
    if ( orcaByteWidth_ != cvSrcImage_->widthStep )
    {
        isPadded_ = true;
    }
    
    // set up opencv storage for the display image
    std::string displayFormat = "BGR8";
    ImageFormat imageDisplayFormat = ImageFormat::find( displayFormat );
    
    numChannels = imageDisplayFormat.getNumberOfChannels();
    depth = imageDisplayFormat.getBitsPerPixel()/numChannels;
    
    cvDisplayImage_ = cvCreateImage( cvSize( width, height ),  depth, numChannels );

    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    
    name_ = "ImageViewer";
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

void Viewer::display( orca::ImageDataPtr& image )
{
    // check the user hasn't closed the window
    if( cvGetWindowHandle( name_ ) != 0 )
    {
        // check if the image has been resized
        if( cvSrcImage_->width != image->description->width && cvSrcImage_->height != image->description->height )
        {
            resize( image );
        }
        
        // check if the opencv image data structure is padded
        if( !isPadded_ )
        {
            // if the opencv structure isn't padded, the image data structures in orca
            // are exactly the same and we just point the opencv image data to
            // the orca image data.
            cvSrcImage_->imageData = (char*)(&image->pixelData[0]);
        }
        else
        {
            // if the opencv image data structure is padded, we have to
            // copy the data line by line into a new area of memory
            // Note that this is less efficient than if the data is not padded
            for(uint32_t j = 0; j < static_cast<uint32_t>( image->description->height ); ++j)
            {
                char* destLine = cvSrcImage_->imageData + j*cvSrcImage_->widthStep;
                unsigned char* srcLine = (&image->pixelData[0]) + j*orcaByteWidth_;
                memcpy(destLine, srcLine, orcaByteWidth_);
            }
        }
        
        // Convert the image format to BGR8 for viewing in an opencv window.
        cvtToBgr( cvSrcImage_, cvDisplayImage_, image->description->format );
        
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
    
void
Viewer::resize( orca::ImageDataPtr& image )
{
    // record required parameters for setting up the resized IplImage
    int srcDepth = cvSrcImage_->depth;
    int srcNumChannels = cvSrcImage_->nChannels;
    int displayDepth = cvDisplayImage_->depth;
    int displayNumChannels = cvDisplayImage_->nChannels;
    // clean up the old IplImages
    cvReleaseImage( &cvSrcImage_ );
    cvReleaseImage( &cvDisplayImage_ );
    
    // create the resized IplImages
    cvSrcImage_ = cvCreateImage( cvSize( image->description->width, image->description->height ), 
                                        srcDepth, 
                                        srcNumChannels );

    cvDisplayImage_ = cvCreateImage( cvSize( image->description->width, image->description->height ), 
                                        displayDepth, 
                                        displayNumChannels );
    
    // check if there's padding with the new image size
    orcaByteWidth_ = image->description->width*cvSrcImage_->nChannels;
    if ( orcaByteWidth_ != cvSrcImage_->widthStep )
    {
        isPadded_ = true;
    }
    
    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
        
}
