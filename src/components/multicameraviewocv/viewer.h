/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef CAMERACOLLECTIONVIEW_OCV_VIEWER_H
#define CAMERACOLLECTIONVIEW_OCV_VIEWER_H

#include <orcaice/context.h>
#include <orca/multicamera.h>

// opencv headers
#include <highgui.h>
#include <cv.h>

#include <string>

namespace multicameraviewocv {
//! @brief Class for viewing multiple images simultaneously using OpenCV. This class sets up the OpenCV structures for storing image formats and displaying them.
class Viewer
{
public:
    Viewer( const orca::MultiCameraDataPtr& multiCameraData,
            const orcaice::Context& context );
    ~Viewer();
    
    //! function that actually displays the image in a window
    void display( orca::MultiCameraDataPtr& images );

private:
    //!
    //! opencv structures
    //!
    //! image structure that stores the image in BGR8 format
    //! for display
    IplImage* cvDisplayImage_;
    // image structure that stores the original image format
    IplImage* cvSrcImage_;
    
    //! handle to the opencv window
    const char* name_;
    
    // display the frame rate of the received images
    void displayFrameRate();
    //! time variables for calculating number of frames per second 
    orca::Time oldFrameTime_;
    orca::Time currentFrameTime_;
    double diff_;
    double fps_;
    //! variable for displaying fps
    CvFont font_;

        
    orcaice::Context context_;
};

} //namespace

#endif
