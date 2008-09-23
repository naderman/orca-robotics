/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "stats.h"
#include <gbxutilacfr/mathdefs.h>
#include <iostream>

using namespace localnav;

Stats::Stats() :
    haveData_(false),
    distance_(0.0),
    timeInMotion_(0.0),
    maxSpeed_(0.0)
{
}

void 
Stats::addData( int seconds, int useconds,
                  const hydronavutil::Pose& pose, 
                  const hydronavutil::Velocity& velocity, 
                  bool isEnabled )
{
    // not keeping track of motion when disabled.
    // force to re-aquire starting point next when next enabled.
    if ( !isEnabled ) {
        haveData_ = false;
        return;
    }

    if ( !haveData_ ) {
        lastSeconds_ = seconds;
        lastUseconds_ = useconds;
        lastPose_ = pose;
        lastIsEnabled_ = isEnabled;
        haveData_ = true;
        return;
    }

    double deltaX = pose.x() - lastPose_.x();
    double deltaY = pose.y() - lastPose_.y();
    double localiserDistance = sqrtf( deltaX*deltaX + deltaY*deltaY );

    const double timeInterval = (seconds-lastSeconds_) + (useconds-lastUseconds_)/1e6;
    const double speed = fabs(velocity.lin());

    const double odometryDistance = speed*timeInterval;
    const double MAX_LOCALISER_JUMP = 10.0;
    if ( localiserDistance < MAX_LOCALISER_JUMP*odometryDistance )
        distance_  += localiserDistance;
    else
        std::cout<<"LocalNavStats: suspect localization jump, not integrating this motion element."<<std::endl;
        // ignore this data

    const double MIN_TIMED_SPEED = 0.10;
    if ( fabs(velocity.lin()) > MIN_TIMED_SPEED )
        timeInMotion_ += timeInterval;

    maxSpeed_ = MAX( maxSpeed_, speed );

    lastSeconds_ = seconds;
    lastUseconds_ = useconds;
    lastPose_ = pose;
    lastIsEnabled_ = isEnabled;
}
