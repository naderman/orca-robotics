/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
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

namespace insgps {

Driver::Config::Config()
{
}

bool
Driver::Config::validate() const
{
    // nothing to validate yet   
    return true;
}

std::string
Driver::Config::toString() const
{
    std::stringstream ss;
    ss << "InsGps driver config: \n";
    ss << "gpsOffset (x,y,z,r,p,y)= \n";
    ss << "\t " << gpsOffset.p.x << ",";
    ss << gpsOffset.p.y << ",";
    ss << gpsOffset.p.z << ",";
    ss << gpsOffset.o.r << ",";
    ss << gpsOffset.o.p << ",";
    ss << gpsOffset.o.y << "\n";
    ss << "imuOffset (x,y,z,r,p,y)= \n";
    ss << "\t " << imuOffset.p.x << ",";
    ss << imuOffset.p.y << ",";
    ss << imuOffset.p.z << ",";
    ss << imuOffset.o.r << ",";
    ss << imuOffset.o.p << ",";
    ss << imuOffset.o.y << "\n";
    ss << "odometry3dOffset (x,y,z,r,p,y)= \n";
    ss << "\t " << odometry3dOffset.p.x << ",";
    ss << odometry3dOffset.p.y << ",";
    ss << odometry3dOffset.p.z << ",";
    ss << odometry3dOffset.o.r << ",";
    ss << odometry3dOffset.o.p << ",";
    ss << odometry3dOffset.o.y << "\n";
    return ss.str();
}

bool 
Driver::Config::operator==( const Driver::Config & other )
{
    return (gpsOffset==other.gpsOffset && gpsSize==other.gpsSize && imuOffset==other.imuOffset && imuSize==other.imuSize && odometry3dOffset==other.odometry3dOffset && odometry3dSize==other.odometry3dSize );
}

bool 
Driver::Config::operator!=( const Driver::Config & other )
{
    return (gpsOffset!=other.gpsOffset && gpsSize!=other.gpsSize && imuOffset!=other.imuOffset && imuSize!=other.imuSize && odometry3dOffset!=other.odometry3dOffset && odometry3dSize!=other.odometry3dSize);
}

} // namespace
