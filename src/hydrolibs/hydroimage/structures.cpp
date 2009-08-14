/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "structures.h"

using namespace hydroimage;


Config::Config()
    :   width(0),
        height(0),
        size(0),
        format("")
{
}

bool
Config::validate() const
{
    if ( width <= 0 ) return false;
    if ( height <= 0 ) return false;
    if ( size <= 0 ) return false;
    if ( format == "" ) return false;
    return true;
}

std::string
Config::toString() const
{
    std::stringstream ss;
    ss << "Image driver config: width=" << width << " height=" << height << " size=" << size << " format=" << format;
    return ss.str();
}

bool 
Config::operator==( const Config & other )
{
    return (width==other.width && height==other.height && size==other.size && format==other.format);
}

bool 
Config::operator!=( const Config & other )
{
    return (width!=other.width || height!=other.height || size!=other.size || format!=other.format); 
}

