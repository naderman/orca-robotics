/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "conversions.h"
#include <orcaimage/imageutils.h>

namespace imageserver {

using namespace std;

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
        cout << "MODE_1024x768_MONO" << endl;
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


} // namespace
