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
#include <hydroutil/mathdefs.h>

// In order to avoid creep on the distance counter when standing still, 
// apply an arbitrary speed threshold. 0.2(m/s)
const double SPEED_THRESHOLD_SQD = 0.20 * 0.20;

using namespace segwayrmp;

Stats::Stats() :
    haveData_(false),
    distance_(0.0),
    timeInMotion_(0.0),
    maxSpeed_(0.0)
{
}

void 
Stats::addData( const hydrointerfaces::SegwayRmp::Data& data )
{
    if ( !haveData_ ) {
        lastData_ = data;
        haveData_ = true;
        return;
    }

    // Note: Velocity is reported in vehicle CS, not arbitary grid like pose.
    // Segway is not able to have sideways velocity, so only need to consider vx
    double speedSqd = data.vx * data.vx;

    // If we are moving, calculate stats
    if ( speedSqd >= SPEED_THRESHOLD_SQD )
    {
        double deltaX = data.x - lastData_.x;
        double deltaY = data.y - lastData_.y;
        distance_ += sqrtf( deltaX*deltaX + deltaY*deltaY );

        timeInMotion_ += (data.seconds-lastData_.seconds) + (data.useconds-lastData_.useconds)/1e6;

        maxSpeed_ = MAX( maxSpeed_, fabs(data.vx) );
    }

    lastData_ = data;
}
