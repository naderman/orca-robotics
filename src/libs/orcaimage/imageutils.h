/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_IMAGEUTILS_H
#define ORCA2_IMAGE_IMAGEUTILS_H

#include <orca/camera.h>

namespace orcaimage{
    
// returns the image format as character array
std::string formatName( const orca::ImageFormat& format );

// number of channels used to store the image
int numChannels( const orca::ImageFormat& format );
}

#endif
