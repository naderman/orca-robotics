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
#include <orcaice/orcaice.h>

#include <orca/pathfollower2d.h>

#include "fakedriver.h"

using namespace std;
using namespace pathplanner;

void 
FakeDriver::computePath( const orca::PathPlanner2dTask& task,
                         orca::PathPlanner2dData& pathData )
{

    orca::Waypoint2d wp;
    pathData.path.resize(5);
    for ( int i=0; i < 5; i++ )
    {
        pathData.path[i] = wp;
    }

    pathData.result = orca::PathOk;
}

