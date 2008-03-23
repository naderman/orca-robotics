/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <sstream>
#include "colourconversions.h"
#include "conversions.h"

#include <orcaimage/imageutils.h>

#ifdef TRICLOPS_FOUND
#   include <triclops/triclops.h>
#   include <triclops/pnmutils.h> 
#endif 

using namespace std;

namespace orcaimage{
 
#ifdef OPENCV_FOUND

void 
cvtToRgb( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src )
{
}

void 
cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src )
{
            memcpy( dest->imageData, &(src->data.at(0)), src->data.size() );
}

#endif

} //namespace
