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
#include <orcaobj/orcaobj.h>

#ifdef TRICLOPS_FOUND
#   include <triclops/triclops.h>
#   include <triclops/pnmutils.h> 
#endif 

using namespace std;

namespace orcaimage{
 
#ifdef OPENCV_FOUND

void 
cvtToRgb( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr  )
{
    switch( descr->format )
    {
        case orca::ImageFormatModeRGB8:
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;
        case orca::ImageFormatModeYUV422:
        case orca::ImageFormatModeBGR8:
        default:
            cout<<"ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaobj::toString( descr->format ) << "." << endl;
            exit(1);
    }
}

void 
cvtToBgr( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr )
{
    switch( descr->format )
    {
        case orca::ImageFormatModeBGR8:
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;

        case orca::ImageFormatModeYUV422:
            // function comes from coriander
            uyvy2bgr( (unsigned char *) (&src->data[0]),
                       (unsigned char *) (dest->imageData), 
                       descr->imageWidth*descr->imageHeight );
            break;

        case orca::ImageFormatModeRGB8:
            rgb2bgr( (unsigned char *) (&src->data[0]),
                      (unsigned char *) (dest->imageData), 
                      descr->imageWidth*descr->imageHeight );
            break;

        case orca::ImageFormatModeGray:
            // do nothing      
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;            
        
        case orca::ImageFormatModeNfi:
            cout << "WARNING(colourconversions.cpp): Assuming image mode " 
                    << orcaobj::toString( descr->format ) << " is in BGR format." << endl;
            cout << "Copying over " << src->data.size() << " bytes" << endl;
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;

        default:
            cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaobj::toString( descr->format ) << "." << endl;
            std::stringstream ss;
            ss << "OrcaImage: unknown image format: " << descr->format<<endl;
            throw( ss.str() );
    }
}

#endif

} //namespace
