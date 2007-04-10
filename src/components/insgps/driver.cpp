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
    ss << "gpsOffset (x,y,o)= \n";
    ss << "\t " << gpsOffset.p.x << ",";
    ss << gpsOffset.p.y << ",";
    ss << gpsOffset.o << "\n";
    ss << "gpsAntennaOffset (x,y,z,r,p,y)= \n";
    ss << "\t " << gpsAntennaOffset.p.x << ",";
    ss << gpsAntennaOffset.p.y << ",";
    ss << gpsAntennaOffset.p.z << ",";
    ss << gpsAntennaOffset.o.r << ",";
    ss << gpsAntennaOffset.o.p << ",";
    ss << gpsAntennaOffset.o.y << "\n";
    ss << "imuOffset (x,y,z,r,p,y)= \n";
    ss << "\t " << imuOffset.p.x << ",";
    ss << imuOffset.p.y << ",";
    ss << imuOffset.p.z << ",";
    ss << imuOffset.o.r << ",";
    ss << imuOffset.o.p << ",";
    ss << imuOffset.o.y << "\n";
//    ss << "imuFlipped = \n";
//    ss << "\t " << imuFlipped << "\n";
    ss << "imuSize (x,y,z) = \n";
    ss << "\t " << imuSize.l << ",";
    ss << imuSize.w << ",";
    ss << imuSize.h << "\n";
    ss << "vehiclePlatformToVehicleTransform (x,y,z,r,p,y)= \n";
    ss << "\t " << vehiclePlatformToVehicleTransform.p.x << ",";
    ss << vehiclePlatformToVehicleTransform.p.y << ",";
    ss << vehiclePlatformToVehicleTransform.p.z << ",";
    ss << vehiclePlatformToVehicleTransform.o.r << ",";
    ss << vehiclePlatformToVehicleTransform.o.p << ",";
    ss << vehiclePlatformToVehicleTransform.o.y << "\n";
    return ss.str();
}

bool 
Driver::Config::operator==( const Driver::Config & other )
{
    return (gpsOffset==other.gpsOffset && gpsAntennaOffset==other.gpsAntennaOffset && imuOffset==other.imuOffset && imuSize==other.imuSize && vehiclePlatformToVehicleTransform==other.vehiclePlatformToVehicleTransform );
}

bool 
Driver::Config::operator!=( const Driver::Config & other )
{
    return (gpsOffset!=other.gpsOffset && gpsAntennaOffset!=other.gpsAntennaOffset && imuOffset!=other.imuOffset && imuSize!=other.imuSize && vehiclePlatformToVehicleTransform!=other.vehiclePlatformToVehicleTransform );
}

} // namespace
