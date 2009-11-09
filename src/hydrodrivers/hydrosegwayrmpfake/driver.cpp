/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <sstream>
#include <IceUtil/IceUtil.h>
#include <gbxutilacfr/gbxutilacfr.h>
#include <hydroiceutil/timeutils.h>
#include "driver.h"

static const double Pi = 3.14159265358979323846264338327950288419717;

using namespace std;

namespace segwayrmpfake
{

Driver::Driver( const std::string        &powerbaseName,
                const hydroutil::Context &context ) 
    : context_(context)
{
    hydroutil::Properties &prop = context_.properties();
    std::string prefix = "Fake.";
    sleepIntervalMs_ = prop.getPropertyAsIntWithDefault( prefix+"SleepInterval", 10 );
    if ( sleepIntervalMs_<0 )
        throw gbxutilacfr::Exception( ERROR_INFO, "Sleep interval must be non-negative." );

    stringstream ss;
    ss << "Fake driver: will simulate data rate with " << sleepIntervalMs_ << "ms inteval";
    context_.tracer().info( powerbaseName_, ss.str() );
}

Driver::~Driver()
{
    context_.tracer().debug( powerbaseName_, "Driver::~Driver()" );
}

void
Driver::enable()
{
    context_.tracer().info( powerbaseName_, "Driver is enabled" );
}

void
Driver::read( Data& data )
{
    const bool jumpAround = false;
    if ( jumpAround )
    {
        data.x      = double(rand()%100000)/1000.0 - 50.0;
        data.y      = double(rand()%100000)/1000.0 - 50.0;
        data.roll   = double(rand()%100000)/1000.0 - 50.0;
        data.pitch  = double(rand()%100000)/1000.0 - 50.0;
        data.yaw    = double(rand()%100000)/1000.0 - 50.0;
        data.vx     = double(rand()%100000)/1000.0 - 50.0;
        data.droll  = double(rand()%100000)/1000.0 - 50.0;
        data.dpitch = double(rand()%100000)/1000.0 - 50.0;
        data.dyaw   = double(rand()%100000)/1000.0 - 50.0;
        data.leftTorque = double(rand()%100000)/1000.0 - 50.0;
        data.rightTorque = double(rand()%100000)/1000.0 - 50.0;
    }
    else
    {
        data.x      = 12.0;
        data.y      = 345.0;
        data.roll   = 0.0;
        data.pitch  = 0.0;
        data.yaw    = 30*M_PI/180.0;
        data.vx     = 0.0;
        data.droll  = 0.0;
        data.dpitch = 0.0;
        data.dyaw   = 0.0;
        data.leftTorque = 0.0;
        data.rightTorque = 0.0;
    }

    data.mainvolt = 80;
    data.uivolt = 9;

    data.hasWarnings = false;
    data.hasFaults = false;

    // simulate reduced data rate
    IceUtil::ThreadControl::sleep( IceUtil::Time::milliSeconds(sleepIntervalMs_) );

    hydroiceutil::now( data.seconds, data.useconds );
}

void
Driver::write( const Command& command )
{
    // debug: simulated failure
    if ( command.vx < 2.0 ) {
        stringstream ss;
        ss << "Driver: Wrote: SegwayRmpCommand (vx,w(deg/s)) : ("
            << command.vx << ", "
            << command.w*180.0/Pi << ")";
        context_.tracer().debug( powerbaseName_, ss.str() );
    }
    else {
        string s = "Simulating hardware failure for velocity over 2m/s";
        context_.tracer().info( powerbaseName_, s );
        throw gbxutilacfr::Exception( ERROR_INFO, s );
    }
}

hydrointerfaces::SegwayRmp::Capabilities
Driver::capabilities() const
{
    hydrointerfaces::SegwayRmp::Capabilities c;
    c.maxForwardSpeed        = 5.0;
    c.maxReverseSpeed        = 5.0;
    c.maxTurnrate            = 999;
    c.maxLateralAcceleration = 3.0;
    return c;
}

} // namespace

extern "C" {
    hydrointerfaces::SegwayRmpFactory *createSegwayRmpDriverFactory()
    { return new segwayrmpfake::Factory; }
}
