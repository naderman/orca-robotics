/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_COLOURCONVERSIONS_H
#define ORCA2_IMAGE_COLOURCONVERSIONS_H

#ifdef OPENCV_FOUND
    #include <opencv/cv.h>
#endif

#include <orca/camera.h>

namespace orcaimage{

#ifdef OPENCV_FOUND
    // This function has not been tested very well
    void cvtToRgb( IplImage* dest, IplImage* bayerSrc, const orca::CameraData& src );
    
    // Often use this function for converting to bgr image format used in opencv
    // TODO: this should only have one IplImage but imaghandler.cpp has been setup so that the bayer 
    // IplImage is differnet to the colour IplImage... is there a way to fix this?
    // Should this function be overloaded so that only one IplImage is an argument?
    void cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::CameraData& src );
#endif
    
} // namespace
    
#endif
