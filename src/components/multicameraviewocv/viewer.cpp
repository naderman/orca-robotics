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

#include <hydroimage/formats.h>
#include <hydroimageocvutil/colourconversions.h>
// for calculating fps
#include <orcaice/timeutils.h>

using namespace multicameraviewocv;
using namespace hydroimage;
using namespace hydroimageocvutil;

Viewer::Viewer( const orca::MultiCameraDataPtr& multiCameraData,
                const orcaice::Context& context ) :
    context_(context)
{
    isPadded_ = false;
    
    // Assume each camera is identical and the image formats are identical
    std::string format = multiCameraData->cameraDataVector.at(0)->description->format;
    int width = multiCameraData->cameraDataVector.at(0)->description->width;
    int height = multiCameraData->cameraDataVector.at(0)->description->height;
    
    // class to search for image format properties
    ImageFormat imageFormat = ImageFormat::find( format );
    int numChannels = imageFormat.getNumberOfChannels();
    int depth = imageFormat.getBitsPerPixel()/numChannels;
    
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
    numChannels  = imageDisplayFormat.getNumberOfChannels();
    depth = imageDisplayFormat.getBitsPerPixel()/numChannels;
    
    // The width of all the camera frames side-by-side
    int totalWidth = cvSrcImage_->width * multiCameraData->cameraDataVector.size();
    
    cvMultiDisplayImage_ = cvCreateImage( cvSize( totalWidth, height ),
                                          depth, 
                                          numChannels );
    
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
    cvReleaseImage( &cvMultiDisplayImage_ );
}

void Viewer::display( orca::MultiCameraDataPtr& images )
{
    // check the user hasn't closed the window
    if( cvGetWindowHandle( name_ ) != 0 )
    {
        for( unsigned int i=0; i<images->cameraDataVector.size(); ++i )
        {
            // check if the opencv image data structure is padded
            if ( !isPadded_ )
            {
                // Point the opencv image data structure to the orca image data
                // Don't want to memcpy to reduce the total copies that occur
                // memcpy( cvImage_->imageData, &(image->data[0]), image->data.size() );
                cvSrcImage_->imageData = (char*)(&images->cameraDataVector.at(i)->pixelData[0]);
            }
            else
            {
                // if the opencv image data structure is padded, we have to
                // copy the data line by line into a new area of memory
                // Note that this is less efficient than if the data is not padded
                for(uint32_t j = 0; j < static_cast<uint32_t>( images->cameraDataVector.at(0)->description->height ); ++j)
                {
                    char* destLine = cvSrcImage_->imageData + j*cvSrcImage_->widthStep;
                    unsigned char* srcLine = (&images->cameraDataVector.at(i)->pixelData[0]) + j*orcaByteWidth_;
                    memcpy(destLine, srcLine, orcaByteWidth_);
                }
            }
            
            // set the region of interest so that each individual image is next to the previous one
            cvSetImageROI( cvMultiDisplayImage_, cvRect(i*cvSrcImage_->width,0,i*cvSrcImage_->width + cvSrcImage_->width, cvSrcImage_->height) );
            
            // Convert the image format to BGR8 for viewing in an opencv window.
            //TODO: change this convert function in hydro so that the format is converted to an openCV format
            // and then call the cvCvtColor function directly
            // Something like: cvCvtColor(src, dest, orcaToOcv(orca::format) )
            cvtToBgr( cvSrcImage_, 
                      cvMultiDisplayImage_, 
                      images->cameraDataVector.at(0)->description->format );
        }
        
        cvResetImageROI( cvMultiDisplayImage_ );
        
        displayFrameRate();
            
        // display the image in an opencv window
        //TODO: need a line displayed separating images
        cvShowImage( name_, cvMultiDisplayImage_ );
            
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
        cvPutText(cvMultiDisplayImage_, fpsString_.str().c_str(), cvPoint(20,20), &font_, CV_RGB(255,255,255)); 
}
    
