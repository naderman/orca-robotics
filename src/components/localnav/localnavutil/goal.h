/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef GOAL_H
#define GOAL_H

#include <cmath>
#include <iostream>

namespace localnav {

//
// Goal location in robot-centric Coord System
//
// @author Alex Brooks
//
class Goal 
{
public:

    // Use this for a particular goal
    void set( double x,
              double y,
              double theta,
              double distanceTolerance,
              double headingTolerance,
              double secRemaining,
              double maxSpeed,
              double maxTurnrate );

    // Use this when no goal is active.
    void setNoGoal();

    double x, y, theta;
    double distanceTolerance;
    double headingTolerance;

    // Time remaining before we're supposed to be at this goal
    double timeRemaining;

    // Constraints
    double maxSpeed;
    double maxTurnrate;
};

// Non-members    
inline double distanceToGoal( const localnav::Goal &goal )
{ return hypotf( goal.y, goal.x ); }
inline double directionToGoal( const localnav::Goal &goal )
{ return atan2( goal.y, goal.x ); }
inline bool translationalGoalPosReached( const localnav::Goal &goal )
{ return ( distanceToGoal(goal) < goal.distanceTolerance ); }
inline bool goalPosReached( const localnav::Goal &goal )
{ return (translationalGoalPosReached(goal) && fabs(goal.theta) < goal.headingTolerance); }
inline bool goalTimeReached( const localnav::Goal &goal )
{ return goal.timeRemaining <= 0; }

std::ostream &operator<<( std::ostream &s, const Goal &g );

}

#endif
