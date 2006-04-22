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
    return (translationalGoalReached() && fabs(goalTheta_) < requiredAngleFromGoal_);
}

bool
GoalWatcher::translationalGoalReached(  ) const
{
    return ( goalDistance_ < requiredDistanceFromGoal_ );
}

}
