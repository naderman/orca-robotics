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

namespace orcaimage
{

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

#ifdef USE_QT
void 
cvtToQImage( QImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr )
{
    IplImage* graytemp = cvCreateImage(cvSize(descr->imageWidth, descr->imageHeight), 8, 1);
    IplImage* colortemp = cvCreateImage(cvSize(descr->imageWidth, descr->imageHeight), 8, 4);
    IplImage* colortemp2 = cvCreateImage(cvSize(descr->imageWidth, descr->imageHeight), 8, 3);
    
    switch( descr->format )
    {
        case orca::ImageFormatBayerBg:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, colortemp2, CV_BayerBG2RGB);
            cvCvtColor(colortemp2, colortemp, CV_RGB2RGBA);
            memcpy( dest->bits(), colortemp->imageData, colortemp->imageSize );
            break;
        case orca::ImageFormatBayerGb:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, colortemp2, CV_BayerGB2RGB);
            cvCvtColor(colortemp2, colortemp, CV_RGB2RGBA);
            memcpy( dest->bits(), colortemp->imageData, colortemp->imageSize );
            break;
        case orca::ImageFormatBayerRg:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, colortemp2, CV_BayerRG2RGB);
            cvCvtColor(colortemp2, colortemp, CV_RGB2RGBA);
            memcpy( dest->bits(), colortemp->imageData, colortemp->imageSize );
            break;
        case orca::ImageFormatBayerGr:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, colortemp2, CV_BayerGR2RGB);
            cvCvtColor(colortemp2, colortemp, CV_RGB2RGBA);
            memcpy( dest->bits(), colortemp->imageData, colortemp->imageSize );
            break;
        case orca::ImageFormatModeBGR8:
            memcpy( colortemp2->imageData, &src->data[0], src->data.size() );
            cvCvtColor( colortemp2, colortemp, CV_BGR2RGBA );
            memcpy( dest->bits(), colortemp->imageData, colortemp->imageSize );
            break;
        case orca::ImageFormatModeYUV422:
            // function comes from coriander
            /*uyvy2rgb( (unsigned char *) (&src->data[0]),
                       (unsigned char *) (dest->bits()), 
                        descr->imageWidth*descr->imageHeight );
            */
            break;
        case orca::ImageFormatModeRGB8:
            memcpy( colortemp2->imageData, &src->data[0], src->data.size() );
            cvCvtColor(colortemp2, colortemp, CV_RGB2RGBA);
            memcpy( dest->bits(), colortemp->imageData, colortemp->imageSize );
            break;
        case orca::ImageFormatModeGray:
            memcpy( graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, colortemp, CV_GRAY2RGBA);
            memcpy(dest->bits(), colortemp->imageData, colortemp->imageSize );  
            break;            
        case orca::ImageFormatModeNfi:
            cout << "WARNING(colourconversions.cpp): Assuming image mode " 
                    << orcaobj::toString( descr->format ) << " is in BGR format." << endl;
            cout << "Copying over " << src->data.size() << " bytes" << endl;
            memcpy( dest->bits(), &src->data[0], src->data.size() );
            break;
        default:
            cout << "ERROR(colourconversions.cpp): Don't know how to convert from image mode " 
                    << orcaobj::toString( descr->format ) << "." << endl;
            std::stringstream ss;
            ss << "OrcaImage: unknown image format: " << descr->format<<endl;
            cvReleaseImage(&graytemp);
            cvReleaseImage(&colortemp);
            cvReleaseImage(&colortemp2);
            throw( ss.str() );
    }

    cvReleaseImage(&graytemp);
    cvReleaseImage(&colortemp);
    cvReleaseImage(&colortemp2);
}

#endif 

void 
cvtToBgr( IplImage* dest, const orca::ImageDataPtr src, const orca::ImageDescriptionPtr descr )
{
    IplImage* graytemp = cvCreateImage(cvSize(descr->imageWidth, descr->imageHeight), 8, 1);

    switch( descr->format )
    {
        case orca::ImageFormatBayerBg:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, dest, CV_BayerBG2BGR);    
            break;
        
        case orca::ImageFormatBayerGb:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, dest, CV_BayerGB2BGR);    
            break;
        case orca::ImageFormatBayerRg:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, dest, CV_BayerRG2BGR);    
            break;

        case orca::ImageFormatBayerGr:
            memcpy(graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, dest, CV_BayerGR2BGR);    
            break;
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
            memcpy( graytemp->imageData, &src->data[0], src->data.size() );
            cvCvtColor(graytemp, dest, CV_GRAY2BGR);
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
            cvReleaseImage(&graytemp);
            throw( ss.str() );
    }

    cvReleaseImage(&graytemp);
}

} //namespace
