/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <gbxutilacfr/mathdefs.h>

#include "laserscanner2d.h"

using namespace std;

namespace hydrointerfaces {

LaserScanner2d::Config::Config()
    :   minRange(0.0),
        maxRange(0.0),
        fieldOfView(0.0),
        startAngle(0.0),
        numberOfSamples(0)
{
}

bool
LaserScanner2d::Config::validate() const
{
    if ( minRange < 0.0 ) return false;
    if ( maxRange <= 0.0 ) return false;
    if ( fieldOfView <= 0.0 || fieldOfView > DEG2RAD(360.0) ) return false;
    if ( startAngle <= DEG2RAD(-360.0) || startAngle > DEG2RAD(360.0) ) return false;
    if ( numberOfSamples <= 0 ) return false;

    return true;
}

std::string
LaserScanner2d::Config::toString() const
{
    std::stringstream ss;
    ss << "Laser driver config: minr="<<minRange<<" maxr="<<maxRange<<" fov="<<RAD2DEG(fieldOfView)<<"deg start="<<RAD2DEG(startAngle)<<"deg num="<<numberOfSamples;
    return ss.str();
}

bool 
LaserScanner2d::Config::operator==( const LaserScanner2d::Config & other )
{
    return (minRange==other.minRange && maxRange==other.maxRange && fieldOfView==other.fieldOfView 
         && startAngle==other.startAngle && numberOfSamples==other.numberOfSamples);
}

bool 
LaserScanner2d::Config::operator!=( const LaserScanner2d::Config & other )
{
    return (minRange!=other.minRange || maxRange!=other.maxRange || fieldOfView!=other.fieldOfView 
         || startAngle!=other.startAngle || numberOfSamples!=other.numberOfSamples);
}

} // namespace
