#include "testsimutil.h"
#include <iostream>
#include <hydroutil/rand.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace localnav {

void getTestPath( orca::PathFollower2dData &d, int numWaypoints )
{
    d.timeStamp = orcaice::getNow();
    orca::Waypoint2d wp;
    int t=0;

    // These settings will be held constant unless otherwise specified.
    wp.distanceTolerance = 1.5;
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
        t += (int) hydroutil::randNum(0,10);
        wp.target.p.x   = hydroutil::randNum(-10,20);
        wp.target.p.y   = hydroutil::randNum(-20,10);
        wp.target.o     = hydroutil::randNum(-M_PI,M_PI);
        wp.timeTarget.seconds  = t;
        wp.timeTarget.useconds = 0;
        wp.maxApproachSpeed = hydroutil::randNum( 1.0, 5.0 );
        wp.maxApproachTurnrate = hydroutil::randNum( 45*M_PI/180.0, 180*M_PI/180.0 );
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

void checkProgress( const orca::PathFollower2dData     &path, 
                    const hydrosim2d::VehicleSimulator &vehicleSim,
                    int                                 iterationNum )
{
    assert( path.path.size() > 0 );

    hydronavutil::Pose pose = vehicleSim.pose();

    const int MIN_NUM_ITERATIONS = 5;
    const int MAX_NUM_ITERATIONS = path.path.size()*400;

    double distanceToLastGoal = hypot( pose.y()-path.path.back().target.p.y,
                                   pose.x()-path.path.back().target.p.x );
    double angleToLastGoal = pose.theta()-path.path.back().target.o;
    NORMALISE_ANGLE( angleToLastGoal );
    angleToLastGoal = fabs( angleToLastGoal );
    bool lastGoalReached = ( distanceToLastGoal <= path.path.back().distanceTolerance &&
                             angleToLastGoal <= path.path.back().headingTolerance );

    if ( iterationNum < MIN_NUM_ITERATIONS && lastGoalReached )
    {
        cout << "ERROR(simulator.cpp): Huh? How did we reach the goal in only "<< iterationNum << " iterations??" << endl;
        exit(1);
    }
    if ( iterationNum >= MAX_NUM_ITERATIONS && !lastGoalReached )
    {
        cout << "ERROR(simulator.cpp): Failed: didn't reach the goal in " <<  iterationNum << " iterations." << endl;
        exit(1);        
    }
    if ( lastGoalReached )
    {
        cout<<"TRACE(simulator.cpp): Final Goal reached.  Test passed." << endl;
        exit(0);
    }

    // Check for collisions
    if ( vehicleSim.isCollided() )
    {
        cout << "ERROR(simulator.cpp): Collision!!" << endl;
        exit(1);
    }
}

}
