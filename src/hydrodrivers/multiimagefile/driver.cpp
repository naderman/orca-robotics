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
#include <cmath>
#include <string>
#include <sstream>
#include <gbxutilacfr/exceptions.h>
#include <stdint.h>

#include <IceUtil/IceUtil.h>

#include <hydroutil/hydroutil.h>

#include "driver.h"

namespace multiimagefile {

Driver::Driver( Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{
    std::string defaultImage = "image.png";
    std::string prefix = "File.";
   
    for( std::vector<int>::size_type i = 0; i < config_.size(); ++i )
    {
        std::stringstream ss;
        ss << i << ".File.";
        filenames_.push_back( context_.properties().getPropertyWithDefault( ss.str() + "FileName", defaultImage ) );
        
        QImage tmpimage(filenames_[i].c_str());
        images_.push_back(tmpimage.convertToFormat(QImage::Format_RGB888));
        if(images_[i].isNull())
        {
            throw gbxutilacfr::Exception(ERROR_INFO, "failed to open file");
        }

        if( images_[i].width() != static_cast<int>(config_[i].width) )
        {
            std::stringstream sse;
            sse << "config [" << i << "] does not match image properties: actual width=" << images_[i].width() << "; config width=" << config_[i].width;
            context_.tracer().warning( sse.str() );
            cfg[i].width = images_[i].width();
            config_[i].width = images_[i].width();
        }

        if( images_[i].height() != static_cast<int>(config_[i].height) )
        {
            std::stringstream sse;
            sse << "config [" << i << "] does not match image properties: actual height=" << images_[i].height() << "; config height=" << config_[i].height;
            context_.tracer().warning( sse.str() );
            cfg[i].height = images_[i].height(); 
            config_[i].height = images_[i].height();
        }

        if( images_[i].numBytes() != static_cast<int>(config_[i].size) )
        {
            std::stringstream sse;
            sse << "config [" << i << "] does not match image properties: actual size=" << images_[i].numBytes() << "; config size=" << config_[i].size;
            context_.tracer().warning( sse.str() );
            cfg[i].size = images_[i].numBytes();
            config_[i].size = images_[i].numBytes();
        }

        if( config_[i].format != "RGB8" )
        {
            std::stringstream sse;
            sse << "config [" << i << "] does not match image properties: actual format=RGB8; config format=" << config_[i].format;
            context_.tracer().warning( sse.str() );
            cfg[i].format = "RGB8";
            config_[i].format = "RGB8";
        }
    }
}

Driver::~Driver()
{
}

void
Driver::read( hydrointerfaces::MultiImage::Data &data )
{
    context_.tracer().debug( "Copying image file data..." );

    //Qt uses a uint32_t array not a byte aligned array so we must cut off the end of each scanline.
    for(std::vector<QImage>::size_type i = 0; i < images_.size(); ++i)
    {
        uint32_t bytesPerLine = static_cast<uint32_t>(images_[i].width()) * 3;
        for(uint32_t j = 0; j < static_cast<uint32_t>(images_[i].height()); ++j)
        {
            memcpy(data[i].pixelData+j*bytesPerLine, images_[i].scanLine(j), bytesPerLine);
        }
        
        IceUtil::Time t = IceUtil::Time::now();
        data[i].timeStampSec = (int)t.toSeconds();
        data[i].timeStampUsec = (int)t.toMicroSeconds() - data[i].timeStampSec*1000000;

    }
}

} // namespace

extern "C" {
    hydrointerfaces::MultiImageFactory *createDriverFactory()
    { return new multiimagefile::Factory; }
}
