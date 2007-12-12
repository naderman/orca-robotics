/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_GOALPLANNER_MAINTHREAD_H
#define ORCA2_GOALPLANNER_MAINTHREAD_H

#include <hydroutil/subsystemthread.h>
#include <orcaice/context.h>
#include <hydroutil/store.h>
#include <hydronavutil/pose.h> 
#include <orca/localise2d.h>
#include <orca/pathplanner2d.h>
#include <orca/pathfollower2d.h>
#include <hydroogmap/hydroogmap.h>
#include "progressmonitor.h"
#include <memory>
#include <orcaifaceimpl/storingconsumers.h>

namespace goalplanner
{

class PathFollower2dI;

class MainThread : public hydroutil::SubsystemThread
{

public: 

    MainThread( const orcaice::Context & context );
    ~MainThread();

    virtual void walk();

private:
    
    void stopRobot();
    void initNetwork();

    // Returns 'gotNewPath':
    //   - if true, we got a new path (written to newPathData).
    //   - if false, we exitted coz the component is stopping.
    bool waitForNewPath( orca::PathFollower2dData &newPathData );

    orca::PathPlanner2dData planPath( const hydronavutil::Pose &pose, const orca::PathFollower2dData &coarsePath );
    orca::PathFollower2dData convertToPathFollowerData( const orca::PathPlanner2dData &pathPlan );
    void sendPath( const orca::PathFollower2dData &pathToSend, bool activateImmediately );

    // Adjust timing: work out how long it takes to the first waypoint based on straight-line distance 
    // and configured velocityToFirstWaypoint_. Take the max of first wp time and the computed time.
    // Add this time to all waypoints.
    void addTimeToReachFirstWp( const hydronavutil::Pose &pose,
                                orca::PathFollower2dData &incomingPath );
    
    // tries a few times to get localise data by issueing remote calls
    void tryGetLocaliseData( orca::Localise2dData &data );
    
    // check whether the localise data is stale, if yes throws an exception
    void checkForStaleness( orca::Localise2dData &data );

    // Gets the most likely pose from the localiser (may throw).
    // Also sets isLocalisationUncertain.
    hydronavutil::Pose getPose( bool &isLocalisationUncertain );

    bool needToReplan( const hydronavutil::Pose &currentPose, const orca::Waypoint2d &currentWp );

    void replan( const hydronavutil::Pose &currentPose, const orca::Waypoint2d &currentWp );

    // required interface to localiser
    orca::Localise2dPrx localise2dPrx_;
    
    // required interface to localnav
    orca::PathFollower2dPrx localNavPrx_;
    
    // required interface to pathplanner
    orca::PathPlanner2dPrx pathplanner2dPrx_;

    // receives and stores information about computed paths 
    orcaifaceimpl::StoringPathPlanner2dConsumerImplPtr computedPathConsumer_;
        
    // ========== provided pathfollower interface (incoming paths) ===============
    PathFollower2dI* incomingPathI_;
    
    hydroutil::Store<orca::PathFollower2dData> incomingPathStore_;
    
    hydroutil::Store<bool> activationStore_;
    // ===========================================================================

    hydroogmap::OgMap ogMap_;

    // For ray-tracing in OG Map
    std::auto_ptr<hydroogmap::OgLosTracer> ogLosTracer_;

    // Monitors how far we are along the path
    ProgressMonitor                 *progressMonitor_;
    Ice::ObjectPtr                   progressMonitorPtr_;
    orca::PathFollower2dConsumerPrx  progressMonitorPrx_;

    bool checkForStaleLocaliseData_;

    // If the path planner takes more than this amount of time, assume something's wrong.
    double pathPlanTimeout_;
    
    // Velocity to get to the first waypoint
    double velocityToFirstWaypoint_;
    orcaice::Context context_;

};

} // namespace

#endif
