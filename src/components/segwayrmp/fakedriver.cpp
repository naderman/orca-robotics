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
using namespace segwayrmp;

FakeDriver::FakeDriver()
{
}

FakeDriver::~FakeDriver()
{
    cout<<"FakeDriver~FakeDriver"<<endl;
}

int FakeDriver::enable()
{
    cout<<"FakeDriver is enabled"<<endl;
    return 0;
}

// int FakeDriver::repair()
// {
//     cout<<"FakeDriver is repaired"<<endl;
//     return 0;
// }

// int FakeDriver::disable()
// {
//     cout<<"FakeDriver is disabled"<<endl;
//     return 0;
// }

int FakeDriver::read( orca::Position2dDataPtr &position2d, orca::Position3dDataPtr &position3d, 
                      orca::PowerDataPtr &power, std::string & status )
{
    orcaice::setSane( position2d );
    orcaice::setSane( position3d );
    orcaice::setSane( power );

    // slow it down a bit
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    status = "faking=1";
    return 0;
}

int FakeDriver::write( const orca::Velocity2dCommandPtr & command )
{
    // debug: simulated failure
    if ( command->motion.v.x < 2.0 ) {
        cout<<"wrote: "<<orcaice::toString(command)<<endl;
        return 0;
    }
    else {
        return 1;
    }
}
