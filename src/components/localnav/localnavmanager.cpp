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
#include "idriver.h"
#include "goal.h"
#include "pathmaintainer.h"
#include <orcanavutil/pose.h>

using namespace std;
using namespace orcaice;

namespace localnav {

//////////////////////////////////////////////////////////////////////
//                    Non-Member  Functions
//////////////////////////////////////////////////////////////////////
float requiredTimeToGoalAtMaxSpeed( const Goal &goal )
{
    assert ( goal.maxSpeed    >= 0.0 );
    assert ( goal.maxTurnrate >= 0.0 );

    const double FOREVER        = 1e9;
    const double LINEAR_EPS     = 1e-5; 
    const double ROTATIONAL_EPS = 1e-5*M_PI/180.0; 

    // The goal covers some area (and range of angles).  How far to the border?
    float distanceToBorder = MAX( 0.0, hypotf(goal.y,goal.x) - goal.distanceTolerance );
    float angleToBorder    = MAX( 0.0, fabs(goal.theta) - goal.headingTolerance );

    // work out how long it would take at max speed
    float translationTime, rotationTime;
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
    if ( angleToBorder == 0.0 )
         rotationTime = 0.0;
    else
    {
        if ( goal.maxTurnrate < ROTATIONAL_EPS )
        {
            return FOREVER;
        }
        else
            rotationTime = angleToBorder / goal.maxTurnrate;
    }
    float requiredTimeAtMaxSpeed = translationTime + rotationTime;
    if ( !(requiredTimeAtMaxSpeed >= 0.0) )
    {
        cout << "ERROR(localnavmanager.cpp): requiredTimeAtMaxSpeed: " << requiredTimeAtMaxSpeed << endl;
        cout<<"TRACE(localnavmanager.cpp): translationTime: " << translationTime << endl;
        cout<<"TRACE(localnavmanager.cpp): rotationTime: " << rotationTime << endl;
    }
    assert( requiredTimeAtMaxSpeed >= 0.0 );
    
//     cout<<"TRACE(localnavmanager.cpp): goal is at: " << goal << endl;
//     cout<<"TRACE(localnavmanager.cpp): reqd time trans, rot = " << translationTime << ", " << rotationTime << endl;

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
      driverState_(IDriver::STATE_GOAL_REACHED),
      context_(context)
{
}
void
setGoalFromWaypoint( const orcanavutil::Pose &pose,
                     const orca::Waypoint2d  &wp,
                     double                   secToWp,
                     Goal                    &goal )
{
    goal.set( wp.target.p.x,
              wp.target.p.y,
              wp.target.o,
              wp.distanceTolerance,
              wp.headingTolerance,
              secToWp,
              wp.maxApproachSpeed,
              wp.maxApproachTurnrate );

    // put the goal in robot's local coord system
    orcanavutil::subtractInitialOffset( goal.x,
                                        goal.y,
                                        goal.theta,
                                        pose.x(),
                                        pose.y(),
                                        pose.theta() );
    NORMALISE_ANGLE( goal.theta );
}

// Returns true iff a waypoint is active
bool
LocalNavManager::getCurrentGoal( const orcanavutil::Pose &pose,
                                 Goal &currentGoal )
{
    while ( true )
    {
        if ( !pathMaintainer_.isActive() )
        {
            return false;
        }

        // Get the current goal
        setGoalFromWaypoint( pose,
                             pathMaintainer_.currentWaypoint(),
                             pathMaintainer_.secToNextWp(),
                             currentGoal );

        if ( goalPosReached(currentGoal) && goalTimeReached(currentGoal) )
        {
            pathMaintainer_.incrementWpIndex();
            // Return to the start of the loop, and peel off a new waypoint.
        }
        else
        {
            // Leave the currentGoal as set
            return true;
        }
    }    
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
    Goal               currentGoal;

    bool haveGoal = getCurrentGoal( pose, currentGoal );
    if ( !haveGoal )
    {
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
        constrainMaxSpeeds( currentGoal, context_ );
        bool obsoleteStall=false;
        driverState_ = driver_.getCommand( obsoleteStall,
                                           odomData.motion,
                                           rangeData,
                                           currentGoal,
                                           cmd );
    }
    // For big debug levels, give feedback through tracer.
    {
        std::stringstream ss;
        ss << "LocalNavManager: Setting command: " << orcaice::toString(cmd);
        context_.tracer()->debug( ss.str(), 5 );
    }
    {
        std::stringstream ss;
        ss << "LocalNavManager: Driver State: " << driverState_;
        context_.tracer()->debug( ss.str(), 2 );
    }
    // TODO: we could maybe do something useful based on the driver's state...
}

std::string 
LocalNavManager::getHeartbeatMessage()
{
    stringstream ss;
    ss << endl;
    ss << "    DriverState: " << driverState_ << endl;
    if ( driverState_ != IDriver::STATE_GOAL_REACHED )
    {
        ss << "    Timing: ";
        if ( secondsBehindSchedule_ > 0.0 )
        {
            ss << "on schedule.";
        }
        else
        {
            ss << "running " << secondsBehindSchedule_ << "s behind schedule.";
        }
    }
    return ss.str();
}

}
