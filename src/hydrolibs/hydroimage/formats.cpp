/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <gbxutilacfr/exceptions.h>
#include "formats.h"

using namespace hydroimage;

std::map<std::string, ImageFormat> ImageFormat::formats_;

ImageFormat::ImageFormat()
: formatString_("")
, channels_(0)
, bitsPerPixel_(0)
, bytesPerPixel_(0)
{
}

ImageFormat::ImageFormat( std::string formatString, unsigned int channels, unsigned int bits, unsigned int bytes ) 
: formatString_( formatString )
, channels_( channels )
, bitsPerPixel_( bits )
, bytesPerPixel_( bytes )
{
   add( *this );
}

const std::string
ImageFormat::getFormatString()
{
    return formatString_;
}

unsigned int
ImageFormat::getNumberOfChannels()
{
    return channels_;
}

unsigned int
ImageFormat::getBitsPerPixel()
{
    return bitsPerPixel_;
}

unsigned int
ImageFormat::getBytesPerPixel()
{
    return bytesPerPixel_;
}

void
ImageFormat::initialize()
{
    static bool initialized = false;
    if(!initialized)
    {
        BayerRG8 bayrg8;
        BayerGR8 baygr8;
        BayerGB8 baygb8;
        BayerBG8 baybg8;
        RGB8 rgb8;
        BGR8 bgr8;
        RGBA8 rgba8;
        BGRA8 bgra8;
        GRAY8 gray8;
		RAW8 raw8;
        initialized = true;
    }
}

ImageFormat
ImageFormat::find( const std::string& format )
{
    initialize();
    std::map<std::string, ImageFormat>::const_iterator iter;
    iter = formats_.find(format);
    if( iter->first != format && iter == formats_.end() )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, "Could not find format");
    }

    return iter->second;
}

void
ImageFormat::add( ImageFormat formatObject )
{
    formats_[formatObject.getFormatString()] = formatObject; 
}
