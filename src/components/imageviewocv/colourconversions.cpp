/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft, Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <sstream>
#include "colourconversions.h"
#include "conversions.h"

// #include <orcaimage/imageutils.h>

using namespace std;

namespace imageviewocv{
 
void 
cvtToRgb( IplImage* dest, const orca::ImageDataPtr& src )
{
    if ( src->description->format == "RGB8" )
    {
        memcpy( dest->imageData, &src->data[0], src->description->size );
        // case orca::ImageFormatModeYuv422:
        // case orca::ImageFormatModeBgr:
    }
    else
    {
        cout<<"ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                << src->description->format << "." << endl;
        exit(1);
    }
}

void 
cvtToBgr( IplImage* dest, const orca::ImageDataPtr& src )
{
    //TODO:  is this dodgy?
    // tmp is pointing to dest.
    // tmp->imageData is then redirected to point to the orca image data
    // so now tmp->imageData is different to dest->imageData but everything
    // else is the same.
    // dest is then changed according to the colour conversion which will
    // change tmp's other parameters. Does this matter once the conversion
    // has started?
    IplImage* tmp = dest;
    tmp->imageData = (char*)(&src->data[0]);
     
    if( src->description->format == "BGR8" )
    {
        memcpy( dest->imageData, &(src->data[0]), src->data.size() );
//         case orca::ImageFormatModeYuv422:
//         // function comes from coriander
//             uyvy2bgr( (unsigned char *) (&src.image[0]),
//                        (unsigned char *) (dest->imageData), 
//                        src.imageWidth*src.imageHeight );
//             break;
    }
    else if( src->description->format == "RGB8" )
    {
//         rgb2bgr( (unsigned char *) (&src->data[0]),
//                 (unsigned char *) (dest->imageData), 
//                 src->description->width*src->description->height );

        cvCvtColor( tmp, dest, CV_RGB2BGR );
    }
    else if( src->description->format == "BGRA8" )
    {
        cvCvtColor( tmp, dest, CV_BGRA2BGR );
    }
    else if( src->description->format == "RGBA8" )
    {
        cvCvtColor( tmp, dest, CV_BGRA2BGR );
    }
    else if( src->description->format == "BayerRG8" )
    {
        cvCvtColor( tmp, dest, CV_BayerRG2BGR );
    }
    else if( src->description->format == "BayerGR8" )
    {
        cvCvtColor( tmp, dest, CV_BayerGR2BGR );
    }
    else if( src->description->format == "BayerGB8" )
    {
        cvCvtColor( tmp, dest, CV_BayerGB2BGR );
    }
    else if( src->description->format == "BayerBG8" )
    {
        cvCvtColor( tmp, dest, CV_BayerBG2BGR );
    }
    else if( src->description->format == "GRAY8" )
    {
        // do nothing
        memcpy( dest->imageData, &(src->data[0]), src->data.size() );
    }
    else
    {
        cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                << src->description->format << "." << endl;
        //TODO: throw an exception
        exit(1);
    }
}


} // namespace

// #endif

