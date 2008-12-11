/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_CAMERACOLLECTIONVIEW_OCV_COLOURCONVERSIONS_H
#define ORCA_CAMERACOLLECTIONVIEW_OCV_COLOURCONVERSIONS_H

// opencv header
#include <cv.h>

#include <orca/image.h>

namespace cameracollectionviewocv{

//! This function has not been tested very well
void cvtToRgb( const IplImage* src, IplImage* dest, const std::string format );

//! Often use this function for converting to bgr image format used in opencv
void cvtToBgr( const IplImage* src, IplImage* dest, const std::string format );
    
} // namespace
    
#endif
