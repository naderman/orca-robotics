/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_SPEEDLIMITER_H
#define ORCA_SPEEDLIMITER_H

#include <orcaice/context.h>
#include <orcaice/heartbeater.h>
#include <orcalocalnav/goal.h>
#include <hydronavutil/hydronavutil.h>

namespace localnav {

class LocalNavParameters;

//
// Constrains the speed according to the next goal taking into account the time
// left remaining to reach that goal.
// The goal is in the robot's local frame of reference.
//
// @author Alex Brooks
//
class SpeedLimiter
{

public: 

    SpeedLimiter( const orcaice::Context &context );

    void setIntendedSpeedThisLeg( const orcalocalnav::Goal &goal );

    // constrain the max speeds for a particular goal
    void constrainMaxSpeeds( orcalocalnav::Goal &goal,
                             const hydronavutil::Velocity &currentVelocity );

private: 

//     void maybeSendHeartbeat( double secondsBehindSchedule );
    
    double intendedSpeedThisLeg_;

//     orcaice::Heartbeater heartbeater_;
    orcaice::Context  context_;
};

}

#endif
