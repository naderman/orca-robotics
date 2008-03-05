/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>

#include "driver.h"

using namespace std;

namespace imageserver {

Driver::Driver( const Config& cfg, const orcaice::Context& context )
    : config_(cfg)
{
};
    
    
int
Driver::initData( orca::CameraData& data ) const
{
    if ( !config_.validate() )
    {
        std::string errString = "Failed to validate camera configuration.";
        context_.tracer().error( errString );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, errString );
        return -1;
    }
    else
    {
        // resize the object to the correct image size
        data.image.resize( config_.imageSize );
        
        // Transfer config info to object info
        data.imageWidth = config_.imageWidth;
        data.imageHeight = config_.imageHeight;
        data.format = config_.format;
        data.compression = config_.compression;

        return 0;
    }
}


Driver::Config::Config()
    :   imageWidth(0),
        imageHeight(0),
        imageSize(0),
        frameRate(0.0),
        format(orca::ImageFormatModeNfi),
        compression(orca::ImageCompressionNone)
{
}

bool
Driver::Config::validate() const
{
    // 0 is default configuration if user has not specified anything
    if ( imageWidth < 0 ) return false;

    if ( imageHeight < 0 ) return false;

    // -1 means use default for the camera   
    if ( frameRate < -1 ) return false;

    return true;
}

std::string
Driver::Config::toString() const
{
    std::stringstream ss;
    ss << "Camera driver config: width="<<imageWidth<<" height="<<imageHeight<<" imageSize="<<imageSize<<" frames="<<frameRate<<" fmt="<<format<<" compr="<<compression;
    return ss.str();
}

bool 
Driver::Config::operator==( const Driver::Config & other )
{
    return (imageWidth==other.imageWidth && imageHeight==other.imageHeight && frameRate==other.frameRate 
         && format==other.format && compression==other.compression);
}

bool 
Driver::Config::operator!=( const Driver::Config & other )
{
    return (imageWidth!=other.imageWidth || imageHeight!=other.imageHeight || frameRate!=other.frameRate 
         || format!=other.format || compression!=other.compression);
}

} // namespace
