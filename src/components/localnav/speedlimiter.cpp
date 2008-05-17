/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>

#include "speedlimiter.h"

using namespace std;
using namespace orcaice;

namespace localnav {

//////////////////////////////////////////////////////////////////////
//                    Non-Member  Functions
//////////////////////////////////////////////////////////////////////

double requiredTimeToGoalAtMaxSpeed( const orcalocalnav::Goal &goal )
{
    assert ( goal.maxSpeed    >= 0.0 );

    const double FOREVER        = 1e9;
    const double LINEAR_EPS     = 1e-5; 

    // The goal covers some area (and range of angles).  How far to the border?
    double distanceToBorder = hypot(goal.y,goal.x) - goal.distanceTolerance;

    // work out how long it would take at max speed
    double translationTime;
    if ( distanceToBorder <= 0.0 )
        translationTime = 0.0;
    else
    {
        if ( goal.maxSpeed < LINEAR_EPS )
        {
            return FOREVER;
        }
        else
            translationTime = distanceToBorder / goal.maxSpeed;
    }
    double requiredTimeAtMaxSpeed = translationTime; // + rotationTime;
    if ( !(requiredTimeAtMaxSpeed >= 0.0) )
    {
        cout << "ERROR(speedlimiter.cpp): requiredTimeAtMaxSpeed: " << requiredTimeAtMaxSpeed << endl;
        cout<<"TRACE(speedlimiter.cpp): translationTime: " << translationTime << endl;
    }
    assert( requiredTimeAtMaxSpeed >= 0.0 );
    return requiredTimeAtMaxSpeed;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

SpeedLimiter::SpeedLimiter( const orcaice::Context& context)
    : secondsBehindSchedule_(0),
      heartbeater_(context),
      context_(context)
{
}

void
SpeedLimiter::constrainMaxSpeeds( orcalocalnav::Goal &goal,
                                  const hydronavutil::Velocity &currentVelocity )
{
    double requiredTimeAtMaxSpeed = requiredTimeToGoalAtMaxSpeed( goal );

    const double AT_GOAL_SPEED_THRESHOLD=0.5;
    if ( orcalocalnav::goalPosReached(goal) && 
         goal.timeRemaining > 0.0 &&
         currentVelocity.lin() < AT_GOAL_SPEED_THRESHOLD )
    {
        // We're at the goal, pretty-much stationary, and we have to wait.
        // constrain the turnrate so we stay put.
        goal.maxSpeed = 0;
        goal.maxTurnrate = 0;
        return;
    }

    // Scale (with a factor in [0,1]) by how long we actually have
    double secondsBehindSchedule;
    if ( goal.timeRemaining > requiredTimeAtMaxSpeed )
    {
        double scaleFactor = requiredTimeAtMaxSpeed / goal.timeRemaining;
        stringstream ss; ss << "We have " << goal.timeRemaining 
                            << "s, but we could get there in " << requiredTimeAtMaxSpeed 
                            << "s if we put the foot down.  Scaling speeds with a factor of " << scaleFactor;
        context_.tracer().debug( ss.str(), 5 );
        secondsBehindSchedule = 0.0;
        assert( scaleFactor <= 1.0 );
        double newSpeed = goal.maxSpeed*scaleFactor;
        const double MIN_SPEED_THRESHOLD = AT_GOAL_SPEED_THRESHOLD*0.95;
        if ( newSpeed < MIN_SPEED_THRESHOLD )
            goal.maxSpeed = MIN( MIN_SPEED_THRESHOLD, goal.maxSpeed );
        else
            goal.maxSpeed = newSpeed;
    }
    else
    {
        stringstream ss; ss << "We're running late! " << goal.timeRemaining
                            << "s allowed, but it would take " << requiredTimeAtMaxSpeed 
                            << "s at full speed.";
        context_.tracer().debug( ss.str(), 5 );
        secondsBehindSchedule = requiredTimeAtMaxSpeed - goal.timeRemaining;
    }

    // Don't constrain the maxTurnrate by timeRemaining.
    // Need to leave the driver with the freedom to swerve hard.
    // goal.maxTurnrate *= scaleFactor;

    maybeSendHeartbeat( secondsBehindSchedule );
}

void
SpeedLimiter::maybeSendHeartbeat( double secondsBehindSchedule )
{
    if ( !heartbeater_.isHeartbeatTime() )
        return;

    stringstream ss;
    ss << "Seeking goal.  Timing: ";
    if ( secondsBehindSchedule_ > 0.0 )
    {
        ss << "on schedule.";
    }
    else
    {
        ss << "running " << secondsBehindSchedule_ << "s behind schedule.";
    }
    
    heartbeater_.beat( ss.str() );
}

}
