/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>

#include "localnavmanager.h"

using namespace std;
using namespace orcaice;

namespace orcalocalnav {

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

float requiredTimeToGoalAtMaxSpeed( const localnav::Goal &goal )
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
constrainMaxSpeeds( localnav::Goal &goal,
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

LocalNavManager::LocalNavManager( const orcaice::Context& context)
    : secondsBehindSchedule_(0),
      heartbeater_(context),
      context_(context)
{
}

bool
LocalNavManager::checkNextGoal( const orca::Localise2dData&        localiseData, 
                                std::vector<localnav::Goal>&       currentGoals,
                                bool&                              uncertainLocalisation,
                                orca::VelocityControl2dData&       cmd )
{
    bool haveGoal = currentGoals.size() > 0;
    maybeSendHeartbeat( haveGoal );
    
    uncertainLocalisation = localisationIsUncertain( localiseData );
    if ( uncertainLocalisation )
        context_.tracer()->warning( "LocalNavManager: Localisation is uncertain..." );
    
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
    }
    else
    {
        constrainMaxSpeeds( currentGoals[0], context_ );
    }
    
    // For big debug levels, give feedback through tracer.
    {
        std::stringstream ss;
        ss << "LocalNavManager: Setting command: " << orcaice::toString(cmd);
        context_.tracer()->debug( ss.str(), 5 );
    }

    return haveGoal;
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
