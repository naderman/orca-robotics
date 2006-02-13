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

#include "teleopfakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using orcaice::operator<<;


TeleopFakeDriver::TeleopFakeDriver( const InputDriver::Config &cfg ) :
        config_(cfg)
{
}

TeleopFakeDriver::~TeleopFakeDriver()
{
}

int TeleopFakeDriver::enable()
{
    cout<<"TeleopFakeDriver is enabled"<<endl;
    return 0;
}

int TeleopFakeDriver::disable()
{
    cout<<"TeleopFakeDriver is disabled"<<endl;
    return 0;
}

int TeleopFakeDriver::readdata( orca::Velocity2dCommandPtr &data )
{
    orcaice::setSane( data );

    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    return 0;
}
