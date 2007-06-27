/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

namespace laser2d {

Driver::Config::Config()
    :   minRange(0.0),
        maxRange(0.0),
        fieldOfView(0.0),
        startAngle(0.0),
        numberOfSamples(0)
{
}

bool
Driver::Config::validate() const
{
    if ( minRange < 0.0 ) return false;
    if ( maxRange <= 0.0 ) return false;
    if ( fieldOfView <= 0.0 || fieldOfView > DEG2RAD(360.0) ) return false;
    if ( startAngle <= DEG2RAD(-360.0) || startAngle > DEG2RAD(360.0) ) return false;
    if ( numberOfSamples <= 0 ) return false;

    return true;
}

std::string
Driver::Config::toString() const
{
    std::stringstream ss;
    ss << "Laser driver config: minr="<<minRange<<" maxr="<<maxRange<<" fov="<<RAD2DEG(fieldOfView)<<"deg start="<<RAD2DEG(startAngle)<<"deg num="<<numberOfSamples;
    return ss.str();
}

bool 
Driver::Config::operator==( const Driver::Config & other )
{
    return (minRange==other.minRange && maxRange==other.maxRange && fieldOfView==other.fieldOfView 
         && startAngle==other.startAngle && numberOfSamples==other.numberOfSamples);
}

bool 
Driver::Config::operator!=( const Driver::Config & other )
{
    return (minRange!=other.minRange || maxRange!=other.maxRange || fieldOfView!=other.fieldOfView 
         || startAngle!=other.startAngle || numberOfSamples!=other.numberOfSamples);
}

} // namespace
