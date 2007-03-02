/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>

#include "localnavmanager.h"
#include <localnavutil/idriver.h>
#include <localnavutil/goal.h>
#include "pathmaintainer.h"
#include <orcanavutil/pose.h>

using namespace std;
using namespace orcaice;

namespace localnav {

//////////////////////////////////////////////////////////////////////
//                    Non-Member  Functions
//////////////////////////////////////////////////////////////////////

bool localisationIsUncertain( const orca::Localise2dData &localiseData )
{
    // Some dodgy heuristics
    if ( localiseData.hypotheses.size() > 2 )
        return true;

    const orca::Pose2dHypothesis h = orcaice::mlHypothesis( localiseData );
    if ( h.cov.xx > 20 ||
         h.cov.yy > 20 )
        return true;

    return false;
}

float requiredTimeToGoalAtMaxSpeed( const Goal &goal )
{
    assert ( goal.maxSpeed    >= 0.0 );

    const double FOREVER        = 1e9;
    const double LINEAR_EPS     = 1e-5; 

    // The goal covers some area (and range of angles).  How far to the border?
    float distanceToBorder = MAX( 0.0, hypotf(goal.y,goal.x) - goal.distanceTolerance );

    // work out how long it would take at max speed
    float translationTime;
    if ( distanceToBorder == 0.0 )
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
    float requiredTimeAtMaxSpeed = translationTime; // + rotationTime;
    if ( !(requiredTimeAtMaxSpeed >= 0.0) )
    {
        cout << "ERROR(localnavmanager.cpp): requiredTimeAtMaxSpeed: " << requiredTimeAtMaxSpeed << endl;
        cout<<"TRACE(localnavmanager.cpp): translationTime: " << translationTime << endl;
    }
    assert( requiredTimeAtMaxSpeed >= 0.0 );
    return requiredTimeAtMaxSpeed;
}

void
constrainMaxSpeeds( Goal &goal,
                    orcaice::Context &context )
{
    double secondsBehindSchedule;
    double requiredTimeAtMaxSpeed = requiredTimeToGoalAtMaxSpeed( goal );

    // Scale (with a factor in [0,1]) by how long we actually have
    double scaleFactor = 1.0;
    if ( goal.timeRemaining > requiredTimeAtMaxSpeed )
    {
        scaleFactor = requiredTimeAtMaxSpeed / goal.timeRemaining;
        stringstream ss; ss << "We have " << goal.timeRemaining 
                            << "s, but we could get there in " << requiredTimeAtMaxSpeed 
                            << "s if we put the foot down.  Scaling speeds with a factor of " << scaleFactor;
        context.tracer()->debug( ss.str(), 5 );
        secondsBehindSchedule = 0.0;
    }
    else
    {
        stringstream ss; ss << "We're running late! " << goal.timeRemaining
                            << "s allowed, but it would take " << requiredTimeAtMaxSpeed 
                            << "s at full speed.";
        context.tracer()->debug( ss.str(), 5 );
        secondsBehindSchedule = requiredTimeAtMaxSpeed - goal.timeRemaining;
    }
    assert( scaleFactor <= 1.0 );
    goal.maxSpeed    *= scaleFactor;

    // Don't constrain the maxTurnrate by timeRemaining.
    // Need to leave the driver with the freedom to swerve hard.
    // goal.maxTurnrate *= scaleFactor;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

LocalNavManager::LocalNavManager( IDriver   &driver,
                                  PathMaintainer   &pathMaintainer,
                                  const orcaice::Context &context)
    : driver_(driver),
      pathMaintainer_(pathMaintainer),
      secondsBehindSchedule_(0),
      heartbeater_(context),
      context_(context)
{
}

orcanavutil::Pose
getMLPose( const orca::Localise2dData &localiseData )
{
    const orca::Pose2dHypothesis &h = orcaice::mlHypothesis( localiseData );
    return orcanavutil::Pose( h.mean.p.x, h.mean.p.y, h.mean.o );
}

void
LocalNavManager::getCommand( const orca::RangeScanner2dDataPtr  rangeData, 
                             const orca::Localise2dData&    localiseData, 
                             const orca::Odometry2dData&    odomData, 
                             orca::VelocityControl2dData&   cmd )
{
    orcanavutil::Pose  pose = getMLPose( localiseData );

    pathMaintainer_.getActiveGoals( currentGoals_,
                                    driver_.waypointHorizon(),
                                    pose );

    bool haveGoal = currentGoals_.size() > 0;
    maybeSendHeartbeat( haveGoal );
    if ( !haveGoal )
    {
        driver_.reset();
        cmd.motion.v.x = 0;
        cmd.motion.v.y = 0;
        cmd.motion.w   = 0;
        {
            std::stringstream ss;
            ss << "LocalNavManager: No active path.  giving command: " << orcaice::toString(cmd);
            context_.tracer()->debug( ss.str(), 5 );
        }
        return;
    }
    else
    {
        constrainMaxSpeeds( currentGoals_[0], context_ );
        bool obsoleteStall=false;
        bool uncertainLocalisation = localisationIsUncertain( localiseData );
        if ( uncertainLocalisation )
            context_.tracer()->warning( "LocalNavManager: Localisation is uncertain..." );

        driver_.getCommand( obsoleteStall,
                            uncertainLocalisation,
                            odomData.motion,
                            rangeData,
                            currentGoals_,
                            cmd );
    }
    // For big debug levels, give feedback through tracer.
    {
        std::stringstream ss;
        ss << "LocalNavManager: Setting command: " << orcaice::toString(cmd);
        context_.tracer()->debug( ss.str(), 5 );
    }
}

void
LocalNavManager::maybeSendHeartbeat( bool haveGoal )
{
    if ( !heartbeater_.isHeartbeatTime() )
        return;

    stringstream ss;
    if ( haveGoal )
    {
        ss << "Seeking goal.  Timing: ";
        if ( secondsBehindSchedule_ > 0.0 )
        {
            ss << "on schedule.";
        }
        else
        {
            ss << "running " << secondsBehindSchedule_ << "s behind schedule.";
        }
    }
    else
    {
        ss << "No active goal.";
    }
    heartbeater_.beat( ss.str() );
}

}
