/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "imageutil.h"
#include "conversions.h"
#include <iostream>

using namespace std;
using namespace orca;

void 
imageviewer::cvtToRgb( char *dest, const orca::CameraDataPtr& src )
{
    switch( src->format )
    {
    case MODERGB:
        memcpy( dest, &src->image[0], src->image.size() );
        break;
    case MODEYUV422:
    case MODEBGR:
    default:
        cout<<"ERROR(imageutil.cpp): Don't know how to convert from image mode " 
            << src->format << "." << endl;
        exit(1);
    }
}

void 
imageviewer::cvtToBgr( char *dest, const orca::CameraDataPtr& src )
{
    switch( src->format )
    {
    case MODEBGR:
        memcpy( dest, &src->image[0], src->image.size() );
        break;
    case MODEYUV422:
        // function comes from libcamera
        uyvy2bgr( (unsigned char *) (src->image[0]),
                  (unsigned char *) (dest), 
                  src->imageWidth*src->imageHeight );
        break;
    case MODERGB:
        rgb2bgr( (unsigned char *) (src->image[0]),
                  (unsigned char *) (dest), 
                  src->imageWidth*src->imageHeight );
        break;
    default:
        cout<<"ERROR(imageutil.cpp): Don't know how to convert from image mode " 
            << src->format << "." << endl;
        exit(1);
    }
}
