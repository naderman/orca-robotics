/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "imageutils.h"

using namespace std;

namespace orcaimage{

orca::ImageFormat orcaImageMode( int mode )
{
    switch( mode ) 
    {
    case MODE_320x240_YUV422:
    case MODE_640x480_YUV422:
    case MODE_800x600_YUV422:
    case MODE_1024x768_YUV422:
    case MODE_1280x960_YUV422:
    case MODE_1600x1200_YUV422:
        return orca::ImageFormatModeYuv422;
    case MODE_640x480_RGB:
    case MODE_800x600_RGB:
    case MODE_1024x768_RGB:
    case MODE_1280x960_RGB:
    case MODE_1600x1200_RGB:
        return orca::ImageFormatModeRgb;
    case MODE_640x480_MONO:
    case MODE_800x600_MONO:
    case MODE_1024x768_MONO:
    case MODE_1280x960_MONO:
    case MODE_1600x1200_MONO:
        return orca::ImageFormatModeGray;
    default:
        orca::ImageFormat format = orca::ImageFormatModeNfi;
        cout << "WARNING(conversions.cpp): Unknown colour mode: " << orcaimage::formatName( format ) << endl;
        return orca::ImageFormatModeNfi;
    }

}

int dc1394ImageMode( orca::ImageFormat mode, int width, int height )
{
    switch( mode )
    {
        case orca::ImageFormatModeYuv422:
            if ( width==320 & height==240 )         
                return MODE_320x240_YUV422;
            else if ( width==640 & height==480 )
                return MODE_640x480_YUV422;
            else if ( width==800 & height==600 )
                return MODE_800x600_YUV422;
            else if ( width==1024 & height==768 )
                return MODE_1024x768_YUV422;
            else if ( width==1280 & height==960 )
                return MODE_1280x960_YUV422;
            else if ( width==1600 & height==1200 )
                return MODE_1600x1200_YUV422;
            else
            {            
                cout << "TRACE(conversions.cpp): This width and height are not supported for YUV" << endl;
                return -1;
            }          
        
        case orca::ImageFormatModeRgb:
            if ( width==640 & height==480 )
                return MODE_640x480_RGB;
            else if ( width==800 & height==600 )
                return MODE_800x600_RGB;
            else if ( width==1024 & height==768 )
                return MODE_1024x768_RGB;
            else if ( width==1280 & height==960 )
                return MODE_1280x960_RGB;
            else if ( width==1600 & height==1200 )
                return MODE_1600x1200_RGB;
            else
            {
                cout << "TRACE(conversions.cpp): This width and height are not supported for RGB" << endl;
                return -1;
            }
            
        case orca::ImageFormatModeGray:
            if ( width==640 & height==480 )
                return MODE_640x480_MONO;
            else if ( width==800 & height==600 )
                return MODE_800x600_MONO;
            else if ( width==1024 & height==768 )
                return MODE_1024x768_MONO;
            else if ( width==1280 & height==960 )
                return MODE_1280x960_MONO;
            else if ( width==1600 & height==1200 )
                return MODE_1600x1200_MONO;
            else
            {
                cout << "TRACE(conversions.cpp): This width and height are not supported for Greyscale" << endl;
                return -1;
            }
        
        case orca::ImageFormatModeNfi:
        default:
            cout << "ERROR(conversions.cpp): Unknown colour mode or ImageFormatModeNfi - cannot convert" << endl;
            return -1;
    }

}

std::string 
formatName( const orca::ImageFormat& format )
{
    switch( format )
    {
        case orca::ImageFormatModeNfi:
            return "ImageFormatModeNfi: Unknown format";
            break;
        case orca::ImageFormatModeGray:  
            return "ImageFormatModeGray: Grayscale";
            break;
        case orca::ImageFormatModeRgb:   
            return "ImageFormatModeRgb: RGB";
            break;
        case orca::ImageFormatModeBgr:   
            return "ImageFormatModeBgr: BGR";
            break;
        case orca::ImageFormatModeYuv422:
            return "ImageFormatModeUv422: YUV 4:2:2";
            break;
        case orca::ImageFormatBayerBg:  
            return "ImageFormatBayerBg: Bayer encoded";
            break;
        case orca::ImageFormatBayerGb:   
            return "ImageFormatBayerGb: Bayer encoded";
            break;
        case orca::ImageFormatBayerRg:   
            return "ImageFormatBayerRg: Bayer encoded";
            break;
        case orca::ImageFormatBayerGr:
            return "ImageFormatBayerGr: Bayer encoded";
            break;
        case orca::ImageFormatDigiclopsStereo:
            return "ImageFormatDigiclopsStereo: All 3 images in RGBU format";
            break;
        case orca::ImageFormatDigiclopsRight:
            return "ImageFormatDigiclopsRight: Right camera image";
            break;
        case orca::ImageFormatDigiclopsBoth:
            return "ImageFormatDigiclopsBoth: All 3 images in RGBU format and the right camera image";
            break;
        default:
            return "WARNING(imageutils.cpp): Image format is not defined";
    }               
}

int 
numChannels( const orca::ImageFormat& format )
{
    switch( format )
    {
        case orca::ImageFormatModeGray:  
        case orca::ImageFormatBayerBg:  
        case orca::ImageFormatBayerGb:   
        case orca::ImageFormatBayerRg:   
        case orca::ImageFormatBayerGr:
            return 1;
            break;
        case orca::ImageFormatModeNfi:
            cout << "WARNING(imageutils.cpp): Assuming image mode " 
                    << orcaimage::formatName( format ) << " is in BGR format." << endl;
            return 3;
            break;              
        default:
            return 3;              
    }                     
}       


} // namespace
