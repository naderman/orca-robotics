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

using namespace imageviewocv;
using namespace hydroimage;

Viewer::Viewer( const int width,
                const int height,
                const std::string& format,
                const orcaice::Context& context ) :
    context_(context)
{
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
    
    name_ = "ImageViewer";
    cvNamedWindow( name_ );
    // context_.tracer()->debug("opencv window created",5);
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
        
        // Point the opencv image data structure to the orca image data
        // Don't want to memcpy to reduce the total copies that occur
        // memcpy( cvImage_->imageData, &(image->data[0]), image->data.size() );
        cvSrcImage_->imageData = (char*)(&image->data[0]);
        
        // Convert the image format to BGR8 for viewing in an opencv window.
        cvtToBgr( cvSrcImage_, cvDisplayImage_, image->description->format );
        
        // display the image in an opencv window
        cvShowImage( name_, cvSrcImage_ );
    
        // Without this, the window doesn't display propery... what the hell?
        cvWaitKey(5);
    }
    else
    {
        // no window to display in so shutdown the component
        context_.communicator()->shutdown();
    }
    
}
