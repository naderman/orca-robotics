/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGE_COLOURCONVERSIONS_H
#define ORCA_IMAGE_COLOURCONVERSIONS_H

#ifdef OPENCV_FOUND
#   include <cv.h>
#endif

#include <orca/camera.h>
#include <orca/image.h>

namespace orcaimage{

#ifdef OPENCV_FOUND
    //! This function has not been tested very well
    void cvtToRgb( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr );
    
    //! Often use this function for converting to bgr image format used in opencv
    void cvtToBgr( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr );

#endif
    
} // namespace
    
#endif
