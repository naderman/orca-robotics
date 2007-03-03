/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHMAINTAINER_H
#define ORCA_PATHMAINTAINER_H

#include <orca/pathfollower2d.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>
#include <orcanavutil/pose.h>
#include <localnavutil/goal.h>

namespace orcalocalnav {

class PathFollower2dI;
class Clock;

//
// @author Alex Brooks
//
// This thing is responsible for keeping track of where we are in the path.
// It doesn't need any mutexes, because all accesses are made by the MainLoop's thread.
// (hence it is _not_ thread-safe).
//
class PathMaintainer
{

public: 

    PathMaintainer( PathFollower2dI           &pathFollowerInterface,
                    const Clock               &clock,
                    const orcaice::Context    &context );

    //
    // Functions for the LocalNavManager
    //

    // Gets a list of up to maxNumGoals goals, the first
    // of which being the one currently sought, given that the robot
    // is at 'pose'.
    //
    // Goals are in robot's local coordinate system.
    // 
    void getActiveGoals( std::vector<localnav::Goal> &goals,
                         int maxNumGoals,
                         const orcanavutil::Pose &pose );

//     const orca::Waypoint2d &currentWaypoint() const;
//     bool  isActive() { return wpIndex_ != -1; }

    //
    // Functions called by MainLoop, to trigger comms with outside world
    //

    void checkForNewPath();
    void checkForWpIndexChange();

//     // Return the remaining time (in seconds) before we have to be at the next waypoint
//     double secToNextWp() const;
    
private: 

    bool waypointReached( const orca::Waypoint2d &wp,
                          const orcanavutil::Pose &pose,
                          double timeNow );
    void  incrementWpIndex();

    // How long since path activation
    double secSinceActivation() const;
    
    // Issue warnings if the path is screwy in some way
    void checkPathOut( const orca::PathFollower2dData& pathData );

    // Keep a local copy of the path, so we don't have to mess with buffers every 
    // time the nav manager wants to look at it.
    orca::PathFollower2dData path_;
    
    // The index of the next waypoint being seeked.
    int wpIndex_;

    // If the index changes, we need to tell the world.
    bool wpIndexChanged_;

    // The timing of the entire path is relative to this
    orca::Time pathStartTime_;

    PathFollower2dI &pathFollowerInterface_;

    // The global time
    const Clock &clock_;

    orcaice::Context context_;
};

}

#endif
