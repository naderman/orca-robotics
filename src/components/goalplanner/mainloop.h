/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_GOALPLANNER_MAINLOOP_H
#define ORCA2_GOALPLANNER_MAINLOOP_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>
#include <hydroutil/proxy.h>
#include <hydronavutil/pose.h> 
#include <orca/localise2d.h>
#include <orca/pathplanner2d.h>
#include <orca/pathfollower2d.h>

namespace goalplanner
{

class PathFollower2dI;

class MainLoop : public hydroutil::Thread
{

public: 

    MainLoop( const orcaice::Context & context );
    ~MainLoop();

    virtual void run();

private:
    
    void stopRobot();
    void initNetwork();
    void computeAndSendPath( const hydronavutil::Pose &pose, const orca::PathFollower2dData &incomingPath );

    // required interface to localiser
    orca::Localise2dPrx localise2dPrx_;
    
    // required interface to localnav
    orca::PathFollower2dPrx localNavPrx_;
    
    // required interface to pathplanner
    orca::PathPlanner2dPrx pathplanner2dPrx_;
    
    // task proxy passed to pathplanner which receives the computed path
    orca::PathPlanner2dConsumerPrx taskPrx_;
    
    // buffer which stores computed path from pathplanner
    hydroutil::Proxy<orca::PathPlanner2dData> computedPathProxy_;
        
    // ========== provided pathfollower interface (incoming paths) ===============
    PathFollower2dI* incomingPathI_;
    
    hydroutil::Proxy<orca::PathFollower2dData> incomingPathProxy_;
    
    hydroutil::Proxy<bool> activationProxy_;
    // ===========================================================================

    // If the path planner takes more than this amount of time, assume something's wrong.
    double pathPlanTimeout_;
    
    // Velocity to get to the first waypoint
    double velocityToFirstWaypoint_;
    void adjustTimes( const hydronavutil::Pose &pose,
                      orca::PathFollower2dData &incomingPath );
    
    // tries a few times to get localise data by issueing remote calls
    void tryGetLocaliseData( orca::Localise2dData &data );
    
    // check whether the localise data is stale, if yes throws an exception
    void checkForStaleness( orca::Localise2dData &data );

    orcaice::Context context_;

};

} // namespace

#endif
