/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>

#include "driver.h"

using namespace std;

namespace imageserver {

Driver::Config::Config()
    :   imageWidth(0),
        imageHeight(0),
        frameRate(0.0),
        format(orca::ImageFormatModeNfi),
        compression(orca::ImageCompressionNone)
{
}

bool
Driver::Config::validate() const
{
    if ( imageWidth <= 0.0 ) return false;

    if ( imageHeight <= 0.0 ) return false;

    if ( frameRate <= 0.0 ) return false;

    return true;
}

std::string
Driver::Config::toString() const
{
    std::stringstream ss;
    ss << "Camera driver config: width="<<imageWidth<<" height="<<imageHeight<<" frames="<<frameRate<<" fmt="<<format<<" compr="<<compression;
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
