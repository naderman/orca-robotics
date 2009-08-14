/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QDebug>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <gbxutilacfr/exceptions.h>
#include <stdint.h>

#include <IceUtil/IceUtil.h>

#include <hydroutil/hydroutil.h>

#include <hydroimage/formats.h>

#include "driver.h"

using namespace std;

namespace imagefile {

Driver::Driver( Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{

    std::string prefix = "File.";
    std::stringstream ss;
    ss << prefix;
    std::stringstream ss_default;
    ss_default << "image.png";
    std::string filename_ = context_.properties().getPropertyWithDefault(ss.str() + "FileName", ss_default.str() );
    QImage tmpimage(filename_.c_str());
    image_ = tmpimage.convertToFormat(QImage::Format_RGB888);

    if(image_.isNull())
    {
        throw gbxutilacfr::Exception(ERROR_INFO, "failed to open file");
    }

    if( image_.width() != static_cast<int>(config_.width) )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual width=" << image_.width() << "; config width=" << config_.width;
        context_.tracer().warning( sse.str() );
        cfg.width = image_.width();
        config_.width = image_.width();
    }

    if( image_.height() != static_cast<int>(config_.height) )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual height=" << image_.height() << "; config height=" << config_.height;
        context_.tracer().warning( sse.str() );
        cfg.height = image_.height(); 
        config_.height = image_.height();
    }

    if( image_.numBytes() != static_cast<int>(config_.size) )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual size=" << image_.numBytes() << "; config size=" << config_.size;
        context_.tracer().warning( sse.str() );
        cfg.size = image_.numBytes();
        config_.size = image_.numBytes();
    }
    
    if( config_.format != "RGB8" )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual format=RGB8; config format=" << config_.format;
        context_.tracer().warning( sse.str() );
        cfg.format = "RGB8";
        config_.format = "RGB8";
    }
}

Driver::~Driver()
{
}

void
Driver::read( hydrointerfaces::Image::Data &data )
{
    context_.tracer().debug( "Copying image file data..." );

    //Qt uses a uint32_t array not a byte aligned array so we must cut off the end of each scanline.
    uint32_t bytesPerLine = static_cast<uint32_t>(image_.width()) * 3;
    for(uint32_t i = 0; i < static_cast<uint32_t>(image_.height()); i++)
    {
        memcpy(data.pixelData+i*bytesPerLine, image_.scanLine(i), bytesPerLine);
    }

    IceUtil::Time t = IceUtil::Time::now();
    data.timeStampSec = (int)t.toSeconds();
    data.timeStampUsec = (int)t.toMicroSeconds() - data.timeStampSec*1000000;
}

} // namespace

extern "C" {
    hydrointerfaces::ImageFactory *createDriverFactory()
    { return new imagefile::Factory; }
}
