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

#include "fakedriver.h"
#include <gbxsickacfr/gbxutilacfr/mathdefs.h>

// Random doubles and integers
#define RDOUBLE (rand()*M_PI)
#define RINT    (rand())
#define RCHAR   ((char)rand())

using namespace std;
using namespace reactivewalker;


int 
FakeDriver::computeCommand( const orca::RangeScanner2dDataPtr& laserData,
                                const orca::Odometry2dData& odometryData,
                                orca::VelocityControl2dData& commandData )
{
    // generate random command
    commandData.motion.v.x = double(RINT%10000)/1000.0 - 30.0;
    commandData.motion.v.y = double(RINT%10000)/1000.0 - 30.0;
    commandData.motion.w = DEG2RAD( double(RINT%100000)/1000.0 - 50.0 );

    return 0;
}
