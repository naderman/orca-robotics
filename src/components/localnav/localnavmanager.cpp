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
#include "localnavdriver.h"
#include "goalwatcher.h"
#include "pathmaintainer.h"

using namespace std;
using namespace orcaice;

namespace localnav {

LocalNavManager::LocalNavManager( LocalNavDriver   &localNavDriver,
                                  GoalWatcher      &goalWatcher,
                                  PathMaintainer   &pathMaintainer,
                                  orcaice::Context  context)
    : localNavDriver_(localNavDriver),
      goalWatcher_(goalWatcher),
      pathMaintainer_(pathMaintainer),
      secondsBehingSchedule_(0),
      driverState_(LocalNavDriver::STATE_GOAL_REACHED),
      context_(context)
{
    // Load the driver up with default nav parameters
    LocalNavParameters defaultParams;
    localNavDriver_.setLocalNavParameters( defaultParams );
}

LocalNavManager::~LocalNavManager()
{
}

void 
LocalNavManager::setCurrentGoal( const orca::Localise2dDataPtr localiseData )
{
    orca::Waypoint2d    currentWaypoint;

    while ( true )
    {
        if ( !pathMaintainer_.isActive() )
        {
            goalWatcher_.setNoGoal();
            LocalNavParameters  noGoalNavParams;
            noGoalNavParams.setToNoGoal();
            localNavDriver_.setLocalNavParameters( noGoalNavParams );
            break;
        }

        // Get the current waypoint
        currentWaypoint = pathMaintainer_.currentWaypoint();

        // Set its constraints
        goalWatcher_.setDistanceTolerance( currentWaypoint.distanceTolerance );
        goalWatcher_.setAngleTolerance( currentWaypoint.headingTolerance );

        // Set the goal in the robot's local coord-system
        bool wpReached = setGoalSpecifics( localiseData,
                                           currentWaypoint,
                                           goalWatcher_ );

        if ( wpReached )
        {
            pathMaintainer_.incrementWpIndex();
            // Return to the start of the loop, and peel off a new waypoint.
        }
        else
        {
            break;
        }
    }
}

void
LocalNavManager::getCommand( const orca::RangeScannerDataPtr  rangeData, 
                             const orca::Localise2dDataPtr    localiseData, 
                             const orca::Position2dDataPtr    odomData, 
                             orca::Velocity2dCommandPtr      &cmd )
{
    setCurrentGoal( localiseData );

    driverState_ = localNavDriver_.getCommand( odomData->stalled,
                                               odomData->motion,
                                               rangeData,
                                               cmd );

    // For big debug levels, give feedback through tracer.
    {
        std::stringstream ss;
        ss << "LocalNavManager: Setting command: " << cmd;
        context_.tracer()->debug( ss.str(), 5 );
    }
    {
        std::stringstream ss;
        ss << "LocalNavManager: Driver State: " << driverState_;
        context_.tracer()->debug( ss.str(), 2 );
    }
    // TODO: we could do something based on the driver's state...
}

bool
LocalNavManager::setGoalSpecifics( const orca::Localise2dDataPtr  localiseData,
                                   const orca::Waypoint2d        &currentWaypoint,
                                   GoalWatcher                   &goalWatcher )
{
    // 1: work out the max-likelihood pose of the robot
    const orca::Pose2dHypothesis &h = orcaice::mlHypothesis( localiseData );
    
    // 2: work out the location of the goal in robot-centric coordinates
    orcanavutil::Offset goal( currentWaypoint.target.p.x, currentWaypoint.target.p.y, currentWaypoint.target.o );
    orcanavutil::subtractInitialOffset( goal.x,
                                    goal.y,
                                    goal.theta,
                                    h.mean.p.x,
                                    h.mean.p.y,
                                    h.mean.o );
    orcanavutil::normaliseAngle( goal.theta );
    goalWatcher.setGoal( goal.x, goal.y, goal.theta );
    context_.tracer()->debug( std::string("Goal in local coords: ") + toString(goal), 5 );

    if ( goalWatcher.goalReached() )
    {
        return true;
    }
    else
    {
        // 3: work out how fast to go, given the time constraints
        LocalNavParameters navParams;
        setNavParams( goalWatcher,
                      currentWaypoint,
                      pathMaintainer_,
                      navParams );
        assert( navParams.maxSpeed >= 0.0 );
        assert( navParams.maxTurnrate >= 0.0 );
        localNavDriver_.setLocalNavParameters( navParams );
        return false;
    }
}

void
LocalNavManager::setNavParams( const GoalWatcher      &goalWatcher,
                               const orca::Waypoint2d &wp, 
                               const PathMaintainer   &pathMaintainer, 
                               LocalNavParameters     &navParams )
{
    cout<<"TRACE(localnavmanager.cpp): setNavParams()" << endl;

    // The goal covers some area (and range of angles).  How far to the border?
    float distanceToBorder = MAX( 0.0, goalWatcher.goalDistance() - goalWatcher.goalDistanceTolerance() );
    float angleToBorder    = MAX( 0.0, fabs(goalWatcher.headingDifference()) - goalWatcher.headingDifferenceTolerance() );

    // work out how long it would take at max speed
    float translationTime = distanceToBorder / wp.maxApproachSpeed;
    float rotationTime = angleToBorder / wp.maxApproachTurnrate;
    float timeAtMaxSpeed = translationTime + rotationTime;
    
    assert( timeAtMaxSpeed >= 0.0 );

    // Scale by how long we actually have
    float timeAllowed = pathMaintainer_.secToNextWp();
    float scaleFactor = 1.0;
    if ( timeAllowed > timeAtMaxSpeed )
    {
        scaleFactor = timeAtMaxSpeed / timeAllowed;
        stringstream ss; ss << "We have " << timeAllowed 
                            << "s, but we could get there in " << timeAtMaxSpeed 
                            << "s if we put the foot down.  Scaling speeds with a factor of " << scaleFactor;
        context_.tracer()->debug( ss.str(), 5 );
        secondsBehingSchedule_ = 0.0;
    }
    else
    {
        stringstream ss; ss << "We're running late! " << timeAllowed << "s allowed, but it would take " << timeAtMaxSpeed << "s at full speed.";
        context_.tracer()->debug( ss.str(), 5 );
        secondsBehingSchedule_ = timeAtMaxSpeed - timeAllowed;
    }
    navParams.maxSpeed    = wp.maxApproachSpeed    * scaleFactor;
    navParams.maxTurnrate = wp.maxApproachTurnrate * scaleFactor;

    cout<<"TRACE(localnavmanager.cpp): setNavParams() exitting." << endl;
}

std::string 
LocalNavManager::getHeartbeatMessage()
{
    stringstream ss;
    ss << endl;
    ss << "    DriverState: " << driverState_ << endl;
    if ( driverState_ != LocalNavDriver::STATE_GOAL_REACHED )
    {
        ss << "    Timing: ";
        if ( secondsBehingSchedule_ > 0.0 )
        {
            ss << "on schedule.";
        }
        else
        {
            ss << "running " << secondsBehingSchedule_ << "s behind schedule.";
        }
    }
    return ss.str();
}

}
