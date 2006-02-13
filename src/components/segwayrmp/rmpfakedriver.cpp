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

#include "rmpfakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

RmpFakeDriver::RmpFakeDriver()
{
}

RmpFakeDriver::~RmpFakeDriver()
{
    cout<<"RmpFakeDriver~RmpFakeDriver"<<endl;
}

int RmpFakeDriver::enable()
{
    cout<<"RmpFakeDriver is enabled"<<endl;
    return 0;
}

int RmpFakeDriver::disable()
{
    cout<<"RmpFakeDriver is disabled"<<endl;
    return 0;
}

int RmpFakeDriver::read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power )
{
    orcaice::setSane( position2d );
    orcaice::setSane( power );

    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int RmpFakeDriver::sendMotionCommand( orca::Velocity2dCommandPtr & command )
{
    // debug: throw exceptions to test client's response
    if ( command->motion.v.x < 2.0 ) {
        cout<<"wrote: "<<orcaice::toString(command)<<endl;
    }
    else {       
        orca::HardwareFailedException e;
        e.what = "throwing pretend exception from RMP fake driver";
        throw e;
    }
    return 0;
}
