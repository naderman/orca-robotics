/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGEOCV_UTIL_COLOURCONVERSIONS_H
#define HYDRO_IMAGEOCV_UTIL_COLOURCONVERSIONS_H

// opencv header
#include <cv.h>
#include <highgui.h>
//dc1394 header
#include <dc1394/dc1394.h>

namespace hydroimageocvutil{

//! This function has not been tested very well
void cvtToRgb( const IplImage* src, IplImage* dest, const std::string format );

//! Often use this function for converting to bgr image format used in opencv
void cvtToBgr( const IplImage* src, IplImage* dest, const std::string format );

//! Converts between Bayer and Rgb using dc1394-2
void dc1394ToBgr( const IplImage* src, IplImage* dest, const std::string format );


} // namespace
    
#endif
