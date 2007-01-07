/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef GOALWATCHER_H
#define GOALWATCHER_H

namespace localnav {

//
// Monitors proximity to goal.
// Works entirely in robot-centric coord system.
//
// @author Alex Brooks
//
class GoalWatcher 
{
public:

    GoalWatcher();

    // Set a particular goal
    void setGoal( float x, float y, float theta );

    // Call this when no goal is active.
    void setNoGoal();

    void setDistanceTolerance( float requiredDistanceFromGoal );
    void setAngleTolerance( float requiredAngleFromGoal );

    bool goalReached() const;
    bool translationalGoalReached() const;

    // distance to centre of goal
    float goalDistance() const { return goalDistance_; }
    float goalDistanceTolerance() const { return requiredDistanceFromGoal_; }

    // direction from robot centre to goal centre 
    float goalDirection() const { return goalDirection_; }

    // goal heading minus robot heading
    float headingDifference() const { return goalTheta_; }
    float headingDifferenceTolerance() const { return requiredAngleFromGoal_; }
    

private:
    float goalX_, goalY_, goalTheta_;
    float goalDistance_;
    float goalDirection_;
    float requiredDistanceFromGoal_;
    float requiredAngleFromGoal_;

};

}

#endif
