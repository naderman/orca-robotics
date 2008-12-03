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
    if( src->description->format == "BGR8" )
    {
        memcpy( dest->imageData, &(src->data[0]), src->data.size() );
//         case orca::ImageFormatModeYuv422:
//         // function comes from coriander
//             uyvy2bgr( (unsigned char *) (&src.image[0]),
//                        (unsigned char *) (dest->imageData), 
//                        src.imageWidth*src.imageHeight );
//             break;
//         case orca::ImageFormatModeRgb:
//             rgb2bgr( (unsigned char *) (&src.image[0]),
//                       (unsigned char *) (dest->imageData), 
//                       src.imageWidth*src.imageHeight );
//             break;
//         // check if the image is bayer encoded
//         case orca::ImageFormatBayerBg:
//         case orca::ImageFormatBayerGb:
//         case orca::ImageFormatBayerRg:
//         case orca::ImageFormatBayerGr:
//         #ifdef OPENCV_FOUND
//              // copy the data from the camera object into the opencv structure
//             memcpy( bayerSrc->imageData, &src.image[0], src.image.size() );
//             
//             // decode and convert to colour using opencv functions
//             switch( src.format )
//             {
//                 case orca::ImageFormatBayerBg:
//                     cvCvtColor( bayerSrc, dest, CV_BayerBG2BGR );
//                     break;              
//                 case orca::ImageFormatBayerGb:
//                     cvCvtColor( bayerSrc, dest, CV_BayerGB2BGR );
//                     break;              
//                 case orca::ImageFormatBayerRg:
//                     cvCvtColor( bayerSrc, dest, CV_BayerRG2BGR );
//                     break;
//                 case orca::ImageFormatBayerGr:
//                     cvCvtColor( bayerSrc, dest, CV_BayerGR2BGR );
//                     break;
//                 default:
//                     break;
//             }
//             break;
//         #endif

//         case orca::ImageFormatModeGray:
//                 // do nothing      
//                 memcpy( dest->imageData, &src.image[0], src.image.size() );
//                 break;            
//         
//         case orca::ImageFormatModeNfi:
//                 cout << "WARNING(colourconversions.cpp): Assuming image mode " 
//                         << orcaimage::formatName( src.format ) << " is in BGR format." << endl;
//                 memcpy( dest->imageData, &src.image[0], src.image.size() );
//                 break;
        }
        else
        {
            cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << src->description->format << "." << endl;
            exit(1);
        }
}

// void 
// cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src )
// {
//     switch( src->format )
//     {
//         case orca::ImageFormatModeBgr:
//             memcpy( dest->imageData, &src->data[0], src->data.size() );
//             break;
// 
//         case orca::ImageFormatModeYuv422:
//             // function comes from coriander
//             uyvy2bgr( (unsigned char *) (&src->data[0]),
//                        (unsigned char *) (dest->imageData), 
//                        src->width*src->height );
//             break;
// 
//         case orca::ImageFormatModeRgb:
//             rgb2bgr( (unsigned char *) (&src->data[0]),
//                       (unsigned char *) (dest->imageData), 
//                       src->width*src->height );
//             break;
// 
//         case orca::ImageFormatModeGray:
//             // do nothing      
//             memcpy( dest->imageData, &src->data[0], src->data.size() );
//             break;            
//         
//         case orca::ImageFormatModeNfi:
//             cout << "WARNING(colourconversions.cpp): Assuming image mode " 
//                     << orcaimage::formatName( src->format ) << " is in BGR format." << endl;
//             memcpy( dest->imageData, &src->data[0], src->data.size() );
//             break;
// 
//         default:
//             cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
//                     << orcaimage::formatName( src->format ) << "." << endl;
//             std::stringstream ss;
//             ss << "OrcaImage: unknown image format: " << src->format<<endl;
//             throw( ss.str() );
//     }

} // namespace

// #endif

