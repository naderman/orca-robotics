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

#include "fakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace segwayrmp;

FakeDriver::FakeDriver( const orcaice::Context& context ) :
    context_(context)
{
}

FakeDriver::~FakeDriver()
{
    cout<<"FakeDriver~FakeDriver"<<endl;
}

int 
FakeDriver::enable()
{
    cout<<"FakeDriver is enabled"<<endl;
    return 0;
}

int 
FakeDriver::disable()
{
    cout<<"FakeDriver is disabled"<<endl;
    return 0;
}

int 
FakeDriver::read( SegwayRmpData& data, std::string &status )
{
    orca::Time t = orcaice::toOrcaTime( IceUtil::Time::now() );
    data.seconds = t.seconds;
    data.useconds = t.useconds;
    data.x = double(rand()%100000)/1000.0 - 50.0;
    data.y = double(rand()%100000)/1000.0 - 50.0;
    data.roll = double(rand()%100000)/1000.0 - 50.0;
    data.pitch = double(rand()%100000)/1000.0 - 50.0;
    data.yaw = double(rand()%100000)/1000.0 - 50.0;
    data.vx = double(rand()%100000)/1000.0 - 50.0;
    data.droll = double(rand()%100000)/1000.0 - 50.0;
    data.dpitch = double(rand()%100000)/1000.0 - 50.0;
    data.dyaw = double(rand()%100000)/1000.0 - 50.0;
    data.mainvolt = double(rand()%100000)/1000.0 - 50.0;
    data.uivolt = double(rand()%100000)/1000.0 - 50.0;

    // slow it down a bit
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    status = "faking=1";
    return 0;
}

int 
FakeDriver::write( const SegwayRmpCommand& command )
{
    // debug: simulated failure
    if ( command.vx < 2.0 ) {
        stringstream ss;
        ss << "Wrote: SegwayRmpCommand (vx,w(deg/s)) : ("
            << command.vx << ", "
            << RAD2DEG(command.w) << ")";
        context_.tracer()->info( ss.str() );
        return 0;
    }
    else {
        return 1;
    }
}
