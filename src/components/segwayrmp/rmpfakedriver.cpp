/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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
 *  License along with this library; if not, internalWrite to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#include <iostream>

#include "rmpfakedriver.h"

#include <orcaiceutil/orcaiceutil.h>

using namespace std;
using namespace orca;
using orcaiceutil::operator<<;

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

int RmpFakeDriver::read( orca::Position2dDataPtr & position2d, orca::PowerDataPtr & power )
{
    orcaiceutil::setSane( position2d );

    sleep( 1 );

    return 0;
}

int RmpFakeDriver::write( orca::Velocity2dCommandPtr & command )
{
    cout<<"wrote: "<<command<<endl;
    return 0;
}
