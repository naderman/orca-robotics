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
cvtToRgb( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr  )
{
    switch( descr->format )
    {
        case orca::ImageFormatModeRgb:
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;
        case orca::ImageFormatModeYuv422:
        case orca::ImageFormatModeBgr:
        default:
            cout<<"ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaimage::formatName( descr->format ) << "." << endl;
            exit(1);
    }
}

void 
cvtToBgr( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr )
{
    switch( descr->format )
    {
        case orca::ImageFormatModeBgr:
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;

        case orca::ImageFormatModeYuv422:
            // function comes from coriander
            uyvy2bgr( (unsigned char *) (&src->data[0]),
                       (unsigned char *) (dest->imageData), 
                       descr->imageWidth*descr->imageHeight );
            break;

        case orca::ImageFormatModeRgb:
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
                    << orcaimage::formatName( descr->format ) << " is in BGR format." << endl;
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;

        default:
            cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaimage::formatName( descr->format ) << "." << endl;
            std::stringstream ss;
            ss << "OrcaImage: unknown image format: " << descr->format<<endl;
            throw( ss.str() );
    }
}

#endif

} //namespace
