/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "goal.h"
#include <iostream>
#include <cmath>
#include <assert.h>

using namespace std;

namespace orcalocalnav {

////////////////////////////////////////////////////////////
// CONSTANTS

// When no goal is set, the driver will try to hold its position
// with these tolerances:
namespace 
{
    const double NO_GOAL_DISTANCE_TOLERANCE = 0.30;
    const double NO_GOAL_HEADING_TOLERANCE  = 30*M_PI/180.0;
}

////////////////////////////////////////////////////////////

void
Goal::set( double x,
           double y,
           double theta,
           double distanceTolerance,
           double headingTolerance,
           double timeRemaining,
           double maxSpeed,
           double maxTurnrate )
{
    this->x = x;
    this->y = y;
    this->theta = theta;
    this->distanceTolerance = distanceTolerance;
    this->headingTolerance  = headingTolerance;
    this->timeRemaining = timeRemaining;
    this->maxSpeed = maxSpeed;
    this->maxTurnrate = maxTurnrate;

    assert( distanceTolerance >= 0.0 );
    assert( headingTolerance >= 0.0 );
    assert( maxSpeed >= 0.0 );
    assert( maxTurnrate >= 0.0 );

    assert( theta >= -M_PI && theta <= M_PI );
}

void
Goal::setNoGoal()
{
    x = 0;
    y = 0;
    theta = 0;
    distanceTolerance = NO_GOAL_DISTANCE_TOLERANCE;
    headingTolerance = NO_GOAL_HEADING_TOLERANCE;      
    timeRemaining = 1e9;
    maxSpeed = 0.01;
    maxTurnrate = 0.01;
}

std::ostream &operator<<( std::ostream &s, const Goal &g )
{
    s << "{["<<g.x<<", "<<g.y<<", "<<g.theta*180.0/M_PI<<"deg] tol=["<<g.distanceTolerance<<", "<<g.headingTolerance*180.0/M_PI<<"deg], timeRem="<<g.timeRemaining<<"s,maxSpeeds=["<<g.maxSpeed<<", "<<g.maxTurnrate*180.0/M_PI<<"]}";
    return s;
}

}
