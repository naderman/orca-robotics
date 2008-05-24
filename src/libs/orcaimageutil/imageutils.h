/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGEUTIL_IMAGEUTILS_H
#define ORCA_IMAGEUTIL_IMAGEUTILS_H

#include <orca/image.h>

#include "conversions.h"

namespace orcaimageutil {
    
// number of channels used to store the image
int numChannels( const orca::ImageFormat& format );

} // namespace

#endif
