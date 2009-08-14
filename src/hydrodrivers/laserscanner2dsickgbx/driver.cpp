/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cstring>
#include <hydroutil/hydroutil.h>
#include <hydroiceutil/hydroiceutil.h>
#include "driver.h"

using namespace std;

namespace laserscanner2dsickgbx {

Driver::Driver( const Config &config, const hydroutil::Context &context ) :
    config_(config),
    context_(context)
{
    // read driver-specific properties
    std::string prefix = "SickGbx.";

    gbxsickacfr::Config gbxConfig;

    gbxConfig.minRange = config.minRange;
    gbxConfig.maxRange = config.maxRange;
    gbxConfig.fieldOfView = config.fieldOfView;
    gbxConfig.startAngle = config.startAngle;
    gbxConfig.numberOfSamples = config.numberOfSamples;

    gbxConfig.baudRate = context_.properties().getPropertyAsIntWithDefault( prefix+"Baudrate", 38400 );

    int ret = context_.properties().getProperty( prefix+"Device", gbxConfig.device );
    if ( ret != 0 )
    {
        stringstream ss;
        ss << "Property was not set: "<<(prefix+"Device");
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    device_.reset( new gbxsickacfr::Driver( gbxConfig, context_.tracer(), context_.status() ) );
}

void 
Driver::read( hydrointerfaces::LaserScanner2d::Data &data )
{
    gbxsickacfr::Data gbxData;
    gbxData.ranges      = &(data.ranges[0]);
    gbxData.intensities = &(data.intensities[0]);

    // catch exceptions!!!
    try 
    {
        device_->read( gbxData );
    
        // ranges and intensities are copied directly into our data structure!
        data.timeStampSec = gbxData.timeStampSec;
        data.timeStampUsec = gbxData.timeStampUsec;
        data.haveWarnings = gbxData.haveWarnings;
        data.warnings = gbxData.warnings;
    }
    catch ( const std::exception& e )
    {
        cout<<"hydro::Driver::read(): Failed to read scan: "<<e.what()<<endl;
        throw;
    }
}

} // namespace

extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createDriverFactory()
    { return new laserscanner2dsickgbx::Factory; }
}
