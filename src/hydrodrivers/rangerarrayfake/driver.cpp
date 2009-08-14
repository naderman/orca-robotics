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

namespace rangerarrayfake {

Driver::Driver( const hydroutil::Context &context ) :
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

    // Set some default values
    description_.minRange = 0.10;
    description_.maxRange = 5.0;
    description_.detectionConeAngle = M_PI / 10;
    for( int i=0; i < 7; i ++)
    {
	double angle = ((i % 2)? 1.0 : -1.0) * M_PI / 6 * ((i + 1) / 2);
	double px = cos(angle);
	double py = sin(angle);
	double pz = 0;
	description_.poses.push_back( hydrobros1::Frame3d( px, py, pz, 0, 0, angle ) );
    }
}

Driver::~Driver()
{
    context_.tracer().debug( "Driver::~Driver()" );
}

void
Driver::getDescription( hydrointerfaces::RangerArray::Description &description )
{
    description = description_;
}


void
Driver::read( hydrointerfaces::RangerArray::Data &data )
{
    context_.tracer().info( "Generating fake ranger array data..." );
    
    IceUtil::Time t = IceUtil::Time::now();

    for ( int i=0; i < (int) description_.poses.size(); i++ )
    {
        data.ranges[i] = 1.0f + i;
        data.timeStampsSec[i] = (int)t.toSeconds();
        data.timeStampsUsec[i] = (int)t.toMicroSeconds() - data.timeStampsSec[i]*1000000;
    }

    IceUtil::ThreadControl::sleep( IceUtil::Time::milliSeconds(sleepIntervalMs_) );

}

} // namespace

extern "C" {
    hydrointerfaces::RangerArrayFactory *createDriverFactory()
    { return new rangerarrayfake::Factory; }
}
