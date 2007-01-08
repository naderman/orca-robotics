#include "testsim.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace localnav {

    void getTestPath( orca::PathFollower2dData &d )
    {
        d.timeStamp = orcaice::getNow();

        orca::Waypoint2d wp;
        wp.target.p.x   = 10;
        wp.target.p.y   = 10;
        wp.target.o     = M_PI;

        wp.distanceTolerance = 1.0;
        wp.headingTolerance  = M_PI/4.0;
        
        wp.timeTarget.seconds  = 0;
        wp.timeTarget.useconds = 0;

        wp.maxApproachSpeed    = 3.0;
        wp.maxApproachTurnrate = 90*M_PI/180.0;

        d.path.push_back( wp );
    }

}
