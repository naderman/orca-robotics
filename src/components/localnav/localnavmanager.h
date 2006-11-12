/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCALNAVMANAGER_H
#define LOCALNAVMANAGER_H

#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orca/localise2d.h>
#include <orca/platform2d.h>
#include <orca/rangescanner2d.h>
#include <orca/pathfollower2d.h>
#include <orcanavutil/orcanavutil.h>
#include "localnavdriver.h"

namespace localnav {

class LocalNavParameters;
class GoalWatcher;
class PathMaintainer;

//
// Takes goals and localisation info in the global reference frame.
// Feeds information to the driver, all in the robot's local frame
// of reference.
//
// Sets the driver's goal using the shared 'GoalWatcher' class.
// Doing things this way ensures that manager and driver don't disagree
// about the goal parameters, and whether it has been reached.
//
// @author Alex Brooks
//
class LocalNavManager
{

public: 

    LocalNavManager( LocalNavDriver   &localNavDriver,
                     GoalWatcher      &goalWatcher,
                     PathMaintainer   &pathMaintainer,
                     orcaice::Context  context );
    ~LocalNavManager();

    // The odometry is required for the velocity, which isn't contained
    // in Localise2d.
    void getCommand( const orca::RangeScanner2dDataPtr  rangeData,
                     const orca::Localise2dDataPtr    localiseData,
                     const orca::Position2dDataPtr    odomData,
                     orca::Velocity2dCommandPtr      &cmd );

    std::string getHeartbeatMessage();

private: 

    void setCurrentGoal( const orca::Localise2dDataPtr localiseData );

    //
    // Sets the goalWatcher and the navParams.
    //
    // Returns: true =  waypoint reached
    //          false = waypoint not reached.
    bool setGoalSpecifics( const orca::Localise2dDataPtr  localiseData,
                           const orca::Waypoint2d        &currentWaypoint,
                           GoalWatcher                   &goalWatcher );

    // Work out how fast to go, given the time constraints
    void setNavParams( const GoalWatcher      &goalWatcher,
                       const orca::Waypoint2d &wp,
                       const PathMaintainer   &pathMaintainer,
                       LocalNavParameters     &navParams );

    // The driver itself
    LocalNavDriver   &localNavDriver_;

    // The goalWatcher: our link for setting the drivers goal and tolerances
    GoalWatcher      &goalWatcher_;

    // This thing is responsible for keeping track of where we are in the path.
    // It's updated asynchronously by the outside world.
    PathMaintainer   &pathMaintainer_;

    orca::Waypoint2d  currentWaypoint_;

    // Maintain these for heartbeat messages
    float                       secondsBehingSchedule_;
    LocalNavDriver::DriverState driverState_;

    orcaice::Context  context_;
};

}

#endif
