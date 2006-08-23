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
FakeDriver::computePath( const orca::PathPlanner2dTaskPtr  & taskPtr,
                         const orca::PathPlanner2dDataPtr  & pathDataPtr )
{

    orca::Waypoint2d wp;
    pathDataPtr->path.resize(5);
    for ( int i=0; i < 5; i++ )
    {
        pathDataPtr->path[i] = wp;
    }

    pathDataPtr->result = orca::PathOk;
    cout << "Fake data: " << endl << pathDataPtr << endl<<endl;

}

