/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "imageutils.h"

using namespace std;

namespace orcaimage{

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
