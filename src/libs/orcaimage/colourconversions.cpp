/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft, Alex Makarenko
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
cvtToRgb( IplImage* dest, IplImage* bayerSrc, const orca::CameraData& src )
{
    switch( src.format )
    {
        case orca::ImageFormatModeRgb:
            memcpy( dest->imageData, &src.image[0], src.image.size() );
            break;
        case orca::ImageFormatModeYuv422:
        case orca::ImageFormatModeBgr:
        default:
            cout<<"ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaimage::formatName( src.format ) << "." << endl;
            exit(1);
    }
}

void 
cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::CameraData& src )
{
    switch( src.format )
    {
        case orca::ImageFormatModeBgr:
            memcpy( dest->imageData, &src.image[0], src.image.size() );
            break;
        case orca::ImageFormatModeYuv422:
        // function comes from coriander
            uyvy2bgr( (unsigned char *) (&src.image[0]),
                       (unsigned char *) (dest->imageData), 
                       src.imageWidth*src.imageHeight );
            break;
        case orca::ImageFormatModeRgb:
            rgb2bgr( (unsigned char *) (&src.image[0]),
                      (unsigned char *) (dest->imageData), 
                      src.imageWidth*src.imageHeight );
            break;
        // check if the image is bayer encoded
        case orca::ImageFormatBayerBg:
        case orca::ImageFormatBayerGb:
        case orca::ImageFormatBayerRg:
        case orca::ImageFormatBayerGr:
        #ifdef OPENCV_FOUND
             // copy the data from the camera object into the opencv structure
            memcpy( bayerSrc->imageData, &src.image[0], src.image.size() );
            
            // decode and convert to colour using opencv functions
            switch( src.format )
            {
                case orca::ImageFormatBayerBg:
                    cvCvtColor( bayerSrc, dest, CV_BayerBG2BGR );
                    break;              
                case orca::ImageFormatBayerGb:
                    cvCvtColor( bayerSrc, dest, CV_BayerGB2BGR );
                    break;              
                case orca::ImageFormatBayerRg:
                    cvCvtColor( bayerSrc, dest, CV_BayerRG2BGR );
                    break;
                case orca::ImageFormatBayerGr:
                    cvCvtColor( bayerSrc, dest, CV_BayerGR2BGR );
                    break;
                default:
                    break;
            }
            break;
        #endif
        case orca::ImageFormatDigiclopsStereo:
        #ifdef TRICLOPS_FOUND
            {
                // TODO: might be able to use the pgr stuff from triclops
                // For now just load into an IplImage struct
                TriclopsInput triclopsInput;
                
                if ( src.format == orca::ImageFormatDigiclopsStereo )
                {
                        triclopsInput.nrows = src.imageHeight;
                        triclopsInput.ncols = src.imageWidth;
                }                   
                else
                {
                    cout << "ERROR(coloutconversions.cpp) Digiclops format: " << orcaimage::formatName( src.format ) << " has not been implemented yet" << endl;
                    exit(1);              
                }
                                                      
                triclopsInput.inputType = TriInp_RGB;
                int imageSize = triclopsInput.nrows * triclopsInput.ncols;

                // set up some temporary storage buffers
                // TODO: do we need to do this?
                std::vector<char> red;
                std::vector<char> green;
                std::vector<char> blue;
                red.resize(imageSize);
                green.resize(imageSize);
                blue.resize(imageSize);

                // the R,G, and B values are stored in separate arrays
                memcpy(&red[0], &src.image[0], imageSize );
                memcpy(&green[0], &src.image[0+imageSize], imageSize );
                memcpy(&blue[0], &src.image[0+2*imageSize], imageSize );

                // TODO: can we copy directly into the triclopsInput?
                triclopsInput.u.rgb.red = &red[0];
                triclopsInput.u.rgb.green = &green[0];
                triclopsInput.u.rgb.blue = &blue[0];

                // char file[] = "test2.ppm";
                // bool res = ppmWriteFromTriclopsInput( file , &triclopsInput );
                // cout << "res: " << res << endl;

                // Triclops Input stores the data from each of the
                // images in 3 separate buffers. Triclops libraries
                // view the data as if each of the bayer encoded images (B&W) was a colour
                // channel. eg. the top camera is the blue channel,
                // left camera=red channel, etc. However opencv
                // requires the rgb data to be interleaved:
                for( int i = 0; i < 3*src.imageHeight*src.imageWidth; i += 3 )
                {
                    dest->imageData[i] = red[i/3];
                    dest->imageData[i+1] = green[i/3];
                    dest->imageData[i+2] = blue[i/3];
                }
            break; 
            }
        #endif

        case orca::ImageFormatModeGray:
                // do nothing      
                memcpy( dest->imageData, &src.image[0], src.image.size() );
                break;            
        
        case orca::ImageFormatModeNfi:
                cout << "WARNING(colourconversions.cpp): Assuming image mode " 
                        << orcaimage::formatName( src.format ) << " is in BGR format." << endl;
                memcpy( dest->imageData, &src.image[0], src.image.size() );
                break;
        default:
            cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaimage::formatName( src.format ) << "." << endl;
            exit(1);
    }
}

void 
cvtToBgr( IplImage* dest, IplImage* bayerSrc, const orca::ImageDataPtr& src )
{
    switch( src->format )
    {
        case orca::ImageFormatModeBgr:
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;

        case orca::ImageFormatModeYuv422:
            // function comes from coriander
            uyvy2bgr( (unsigned char *) (&src->data[0]),
                       (unsigned char *) (dest->imageData), 
                       src->width*src->height );
            break;

        case orca::ImageFormatModeRgb:
            rgb2bgr( (unsigned char *) (&src->data[0]),
                      (unsigned char *) (dest->imageData), 
                      src->width*src->height );
            break;

        case orca::ImageFormatModeGray:
            // do nothing      
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;            
        
        case orca::ImageFormatModeNfi:
            cout << "WARNING(colourconversions.cpp): Assuming image mode " 
                    << orcaimage::formatName( src->format ) << " is in BGR format." << endl;
            memcpy( dest->imageData, &src->data[0], src->data.size() );
            break;

        default:
            cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaimage::formatName( src->format ) << "." << endl;
            std::stringstream ss;
            ss << "OrcaImage: unknown image format: " << src->format<<endl;
            throw( ss.str() );
    }
}

#endif

} //namespace
