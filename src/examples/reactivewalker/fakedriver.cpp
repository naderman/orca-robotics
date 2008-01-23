/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaobj/orcaobj.h>

#include "fakedriver.h"

using namespace std;
using namespace reactivewalker;


int 
FakeDriver::computeCommand( const orca::RangeScanner2dDataPtr& laserData,
                                const orca::Odometry2dData& odometryData,
                                orca::VelocityControl2dData& commandData )
{
    // generate random command
    orcaobj::setSane( commandData );
    
    return 0;
}
