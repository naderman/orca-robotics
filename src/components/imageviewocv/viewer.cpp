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
    
    // opencv gear here
    int depth = imageFormat.getBitsPerPixel()/imageFormat.getNumberOfChannels();
    cvImage_ = cvCreateImage( cvSize( width, height ),  depth, imageFormat.getNumberOfChannels() );
    // dodgy opencv needs this so it has time to resize
    cvWaitKey(100);
    
    name_ = "ImageViewer";
    cvNamedWindow( name_ );
    // context_.tracer()->debug("opencv window created",5);
}

Viewer::~Viewer()
{
    cvDestroyWindow( name_ );
    cvReleaseImage( &cvImage_ );
}

void Viewer::display( orca::ImageDataPtr image )
{

    // check the user hasn't closed the window
    if( cvGetWindowHandle( name_ ) != 0 )
    {
        
        // convert the image format to BGR for viewing in an opencv window. Copy the image data
        // into an opencv IplImage structure
        cvtToBgr( cvImage_, image );
        
        // copy the image data into the IplImage variable from the orca image variable
        // memcpy( cvImage_->imageData, &(image->data[0]), image->data.size() );
        
        // display the image in an opencv window
        cvShowImage( name_, cvImage_ );
    
        // Without this, the window doesn't display propery... what the hell?
        cvWaitKey(5);
    }
    else
    {
        // no window to display in so shutdown the component
        context_.communicator()->shutdown();
    }
    
}
