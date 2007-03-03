#include "testsim.h"
#include <iostream>
#include <orcamisc/rand.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace localnav {

    void getTestPath( orca::PathFollower2dData &d, int numWaypoints )
    {
        d.timeStamp = orcaice::getNow();
        orca::Waypoint2d wp;
        int t=0;

        // These settings will be held constant unless otherwise specified.
        wp.distanceTolerance = 1.0;
        wp.headingTolerance  = M_PI/8.0;
        wp.maxApproachSpeed    = 3.0;
        wp.maxApproachTurnrate = 90*M_PI/180.0;

        
        // Turn on the spot first
        t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = -M_PI/2;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI/2;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        t+=3;
        wp.target.p.x   = 0;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI/2;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        // Then move off
        t+=3;
        wp.target.p.x   = 10;
        wp.target.p.y   = 10;
        wp.target.o     = M_PI;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        t+=3;
        wp.target.p.x   = 10;
        wp.target.p.y   = 0;
        wp.target.o     = M_PI;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        // Try to get thru door
        t+=3;
        wp.target.p.x   = -14;
        wp.target.p.y   = 10;
        wp.target.o     = M_PI/2.0;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        t+=3;
        wp.target.p.x   = -14;
        wp.target.p.y   = 14;
        wp.target.o     = M_PI/2.0;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );
        
        // Then back out.
        t+=3;
        wp.target.p.x   = -14;
        wp.target.p.y   = 14;
        wp.target.o     = -M_PI/2.0;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        t+=3;
        wp.target.p.x   = -14;
        wp.target.p.y   = 10;
        wp.target.o     = -M_PI/2.0;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );

        // Now add a random marathon
        int numRandom=numWaypoints-d.path.size();
        for ( int i=0; i < numRandom; i++ )
        {
            t += (int) orcamisc::randNum(0,10);
            wp.target.p.x   = orcamisc::randNum(-10,10);
            wp.target.p.y   = orcamisc::randNum(-10,10);
            wp.target.o     = orcamisc::randNum(-M_PI,M_PI);
            wp.timeTarget.seconds  = t;
            wp.timeTarget.useconds = 0;
            wp.maxApproachSpeed = orcamisc::randNum( 1.0, 5.0 );
            wp.maxApproachTurnrate = orcamisc::randNum( 45*M_PI/180.0, 180*M_PI/180.0 );
            d.path.push_back( wp );            
        }

        // The test pass/fail criterion is reaching the last goal.  So put it away from the others.
        t+= 5;
        wp.target.p.x   = -15;
        wp.target.p.y   = -15;
        wp.target.o     = 0;
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        d.path.push_back( wp );        
    }

}
