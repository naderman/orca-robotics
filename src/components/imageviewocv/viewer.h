/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMAGEVIEW_OCV_VIEWER_H
#define IMAGEVIEW_OCV_VIEWER_H

// #include "viewer.h"

#include <orcaice/context.h>
#include <orca/image.h>

// opencv headers
#include <highgui.h>
#include <cv.h>

#include <string>

// forward declaration
// class ImageFormat;

namespace imageviewocv {

class Viewer
{
public:
    Viewer( const int width,
            const int height,
            const std::string& format,
            const orcaice::Context& context );
    ~Viewer();
    
    void display( orca::ImageDataPtr image );

private:
    
    IplImage* cvImage_;
    // IplImage* cvImageTmp_;
    // IplImage* bayerImage_;
    
    // handle to the opencv window
    const char* name_;
    
    // The particular image format that is being received
    // ImageFormat imageFormat_;

    orcaice::Context context_;
};

} //namespace

#endif
