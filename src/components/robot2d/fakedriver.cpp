/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#include <iostream>

#include "fakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using namespace robot2d;

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

int FakeDriver::repair()
{
    cout<<"FakeDriver is repaired"<<endl;
    return 0;
}

int FakeDriver::disable()
{
    cout<<"FakeDriver is disabled"<<endl;
    return 0;
}

int FakeDriver::read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power,
                std::string & status )
{
    orcaice::setSane( position2d );
    //orcaice::setSane( power );

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
