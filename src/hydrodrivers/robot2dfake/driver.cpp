/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <IceUtil/IceUtil.h>
#include <hydroiceutil/timeutils.h>
#include "driver.h"

static const double Pi = 3.14159265358979323846264338327950288419717;

using namespace std;

namespace robot2dfake
{

Driver::Driver( const hydroutil::Context& context ) :
    context_(context)
{
}

Driver::~Driver()
{
    cout<<"Driver~Driver"<<endl;
}

void
Driver::enable()
{
    cout<<"Driver is enabled"<<endl;
}

void
Driver::disable()
{
    cout<<"Driver is disabled"<<endl;
}

bool
Driver::read( Data& data )
{
    data.x = double(rand()%100000)/1000.0 - 50.0;
    data.y = double(rand()%100000)/1000.0 - 50.0;
    data.yaw = double(rand()%100000)/1000.0 - 50.0;
    data.vlong = double(rand()%100000)/1000.0 - 50.0;
    data.vtransverse = double(rand()%100000)/1000.0 - 50.0;
    data.dyaw = double(rand()%100000)/1000.0 - 50.0;

    // slow it down a bit
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    hydroiceutil::now( data.seconds, data.useconds );
    return false;
}

void
Driver::write( const Command& command )
{
    // debug: simulated failure
    if ( command.vlong < 2.0 ) {
        stringstream ss;
        ss << "Driver: Wrote: SegwayRmpCommand (vlong,transverse, w(deg/s)) : ("
            << command.vlong << ", "
            << command.vtransverse << ", "
            << command.dyaw*180.0/Pi << ")";
        context_.tracer().info( ss.str() );
    }
    else {
        string s = "Simulating hardware failure for velocity over 2m/s";
        context_.tracer().info( s );
        throw hydrointerfaces::Robot2d::Exception( s );
    }
}

void 
Driver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    status="faking=1";
    isWarn=0;
    isFault=0;
}

void 
Driver::applyHardwareLimits( double &maxForwardSpeed, double &maxReverseSpeed, 
                              double &maxTurnrate, double &maxTurnrateAtMaxSpeed )
{
}

} // namespace

extern "C" {
    hydrointerfaces::Robot2dFactory *createRobot2dDriverFactory()
    { return new robot2dfake::Factory; }
}
