/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_SPEEDLIMITER_H
#define ORCA_SPEEDLIMITER_H

#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orca/velocitycontrol2d.h>
#include <orca/rangescanner2d.h>
#include <orca/pathfollower2d.h>
#include <orcanavutil/orcanavutil.h>
#include <orcaice/heartbeater.h>
#include "goal.h"

namespace orcalocalnav {

class LocalNavParameters;
class Goal;

//
// Constrains the speed according to the next set of current goals.
// The goals are in the robot's local frame of reference.
//
// @author Alex Brooks
//
class SpeedLimiter
{

public: 

    SpeedLimiter( const orcaice::Context &context );

    // constrain the max speeds for a particular goal
    void constrainMaxSpeeds( Goal &goal );

    // set all velocity commands to zero
    void setToZero( orca::VelocityControl2dData&  cmd );

private: 

    void maybeSendHeartbeat();
    
    // Maintain these for heartbeat messages
    double  secondsBehindSchedule_;

    orcaice::Heartbeater heartbeater_;
    orcaice::Context  context_;
};

}

#endif
