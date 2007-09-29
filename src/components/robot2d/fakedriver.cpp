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
using namespace robot2d;

FakeDriver::FakeDriver( const orcaice::Context& context ) :
    context_(context)
{
}

FakeDriver::~FakeDriver()
{
    cout<<"FakeDriver~FakeDriver"<<endl;
}

void 
FakeDriver::enable()
{
    context_.tracer()->info( "FakeDriver is enabled" );
}

bool
FakeDriver::read( Data& data )
{
    context_.tracer()->info( "Generating fake info for robot2d..." );

    orca::Time t = orcaice::toOrcaTime( IceUtil::Time::now() );
    data.seconds = t.seconds;
    data.useconds = t.useconds;
    data.x = double(rand()%100000)/1000.0 - 50.0;
    data.y = double(rand()%100000)/1000.0 - 50.0;
    data.o = double(rand()%100000)/1000.0 - 50.0;
    data.vx = double(rand()%100000)/1000.0 - 50.0;
    data.vy = double(rand()%100000)/1000.0 - 50.0;
    data.w = double(rand()%100000)/1000.0 - 50.0;

    // slow it down a bit
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    return false;
}

void
FakeDriver::write( const Command& command )
{
    // debug: simulated failure
    if ( command.vx < 2.0 ) {
        stringstream ss;
        ss << "Wrote: Command (vx,vy,w(deg/s)) : ("
            << command.vx << ", "
            << command.vy << ", "
            << RAD2DEG(command.w) << ")";
        context_.tracer()->info( ss.str() );
    }
    else {
        throw( orcaiceutil::Exception( ERROR_INFO, "FakeDriver: simulated hardware fault" ) );
    }
}

void 
FakeDriver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    status = "FakeDriver: dummy status";
    isWarn = false;
    isFault = false;
}
