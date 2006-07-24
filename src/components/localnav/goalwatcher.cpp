/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "goalwatcher.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace localnav {

////////////////////////////////////////////////////////////
// CONSTANTS

// When no goal is set, the driver will try to hold its position
// with these tolerances:
static const float NO_GOAL_DISTANCE_TOLERANCE = 0.30;
static const float NO_GOAL_ANGLE_TOLERANCE    = 30*M_PI/180.0;

////////////////////////////////////////////////////////////

GoalWatcher::GoalWatcher()
{
    setNoGoal();
}

void
GoalWatcher::setGoal( float x, float y, float theta )
{
    goalX_ = x; goalY_ = y; goalTheta_ = theta;
    goalDistance_  = hypotf( goalX_, goalY_ );
    goalDirection_ = atan2( goalY_, goalX_ );
}

void
GoalWatcher::setNoGoal()
{
    setGoal( 0, 0, 0 );
    setDistanceTolerance( NO_GOAL_DISTANCE_TOLERANCE );
    setAngleTolerance( NO_GOAL_ANGLE_TOLERANCE );
}

void
GoalWatcher::setDistanceTolerance( float requiredDistanceFromGoal )
{
    requiredDistanceFromGoal_ = requiredDistanceFromGoal;
}

void
GoalWatcher::setAngleTolerance( float requiredAngleFromGoal )
{
    requiredAngleFromGoal_ = requiredAngleFromGoal;
}

bool
GoalWatcher::goalReached() const
{
    bool goalReached = (translationalGoalReached() && fabs(goalTheta_) < requiredAngleFromGoal_);
    return goalReached;
}

bool
GoalWatcher::translationalGoalReached(  ) const
{
    return ( goalDistance_ < requiredDistanceFromGoal_ );
}

}
