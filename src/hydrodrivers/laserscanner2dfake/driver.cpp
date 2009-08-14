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
#include <IceUtil/IceUtil.h>
#include <gbxutilacfr/gbxutilacfr.h>

#include "driver.h"

using namespace std;

namespace laserscanner2dfake {

Driver::Driver( const Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{
    hydroutil::Properties &prop = context_.properties();
    std::string prefix = "Fake.";
    sleepIntervalMs_ = prop.getPropertyAsIntWithDefault( prefix+"SleepInterval", 100 );
    if ( sleepIntervalMs_<0 )
        throw gbxutilacfr::Exception( ERROR_INFO, "Sleep interval must be non-negative." );

    stringstream ss;
    ss << "Fake driver: will simulate data rate with " << sleepIntervalMs_ << "ms inteval";
    context_.tracer().info( ss.str() );
}

Driver::~Driver()
{
    context_.tracer().debug( "Driver::~Driver()" );
}

void
Driver::read( hydrointerfaces::LaserScanner2d::Data &data )
{
    context_.tracer().info( "Generating fake laser data..." );
    
    for ( int i=0; i < config_.numberOfSamples; i++ )
    {
        data.ranges[i] = (float)((i/(float)(config_.numberOfSamples))*config_.maxRange);
        data.intensities[i] = i%2;
    }

    IceUtil::ThreadControl::sleep( IceUtil::Time::milliSeconds(sleepIntervalMs_) );

    IceUtil::Time t = IceUtil::Time::now();
    data.timeStampSec = (int)t.toSeconds();
    data.timeStampUsec = (int)t.toMicroSeconds() - data.timeStampSec*1000000;
}

} // namespace

extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createDriverFactory()
    { return new laserscanner2dfake::Factory; }
}
