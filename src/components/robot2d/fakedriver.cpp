/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>

#include "fakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using namespace robot2d;

FakeDriver::FakeDriver( const orcaice::Context& context ) :
    context_(context)
{
}

FakeDriver::~FakeDriver()
{
    cout<<"FakeDriver~FakeDriver"<<endl;
}

int FakeDriver::enable()
{
    context_.tracer()->info( "FakeDriver is enabled" );
    return 0;
}

int FakeDriver::repair()
{
    context_.tracer()->info( "FakeDriver is repaired" );
    return 0;
}

int FakeDriver::disable()
{
    context_.tracer()->info( "FakeDriver is disabled" );
    return 0;
}

int FakeDriver::read( orca::Position2dData& position2d, std::string& status )
{
    context_.tracer()->info( "Generating fake info for robot2d..." );
    orcaice::setSane( position2d );

    // slow it down a bit
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    status = "faking=1";
    return 0;
}

int FakeDriver::write( const orca::Velocity2dCommand& command )
{
    // debug: simulated failure
    if ( command.motion.v.x < 2.0 ) {
        context_.tracer()->info( "Wrote: "+orcaice::toString(command) );
        return 0;
    }
    else {
        return 1;
    }
}
