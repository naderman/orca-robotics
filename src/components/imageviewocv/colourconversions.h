/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGEVIEWOCV_COLOURCONVERSIONS_H
#define ORCA_IMAGEVIEWOCV_COLOURCONVERSIONS_H

// opencv header
#include <cv.h>

#include <orca/image.h>

namespace imageviewocv{

//! This function has not been tested very well
void cvtToRgb( IplImage* dest, const orca::ImageDataPtr& src );

//! Often use this function for converting to bgr image format used in opencv
void cvtToBgr( IplImage* dest, const orca::ImageDataPtr& src );

//! Same as above but the image comes from the Image interface.
//! Throws a std::string if the format unknown.
// void cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src );
    
} // namespace
    
#endif
