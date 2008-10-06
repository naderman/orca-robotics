/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHMAINTAINER_H
#define ORCA_PATHMAINTAINER_H

#include <orca/pathfollower2d.h>
#include <orcaice/context.h>
#include <hydronavutil/pose.h>
#include "goal.h"

namespace localnav {

class PathFollowerInterface;
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

    PathMaintainer( PathFollowerInterface     &pathFollowerInterface,
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
    // returns FALSE if there are no active goals, and TRUE otherwise
    bool getActiveGoals( std::vector<orcalocalnav::Goal> &goals,
                         int maxNumGoals,
                         const hydronavutil::Pose &pose,
                         bool &wpIncremented );

    //
    // Functions called by MainLoop, to trigger comms with outside world
    //

    void checkForNewPath();
    void checkForWpIndexChange();

private: 

    bool waypointReached( const orca::Waypoint2d &wp,
                          const hydronavutil::Pose &pose,
                          double timeNow );
    void  incrementWpIndex();

    // How long since path activation
    double secSinceActivation() const;
    
    // Keep a local copy of the path, so we don't have to mess with buffers every 
    // time the nav manager wants to look at it.
    orca::PathFollower2dData path_;
    
    // The index of the next waypoint being seeked.
    int wpIndex_;

    // If the index changes, we need to tell the world.
    bool wpIndexChanged_;

    // Used to handle corner-case of 'wpIncremented' on first call to getActiveGoals after new path.
    bool justReceivedNewPath_;

    // The timing of the entire path is relative to this
    orca::Time pathStartTime_;

    PathFollowerInterface &pathFollowerInterface_;

    // The global time
    const Clock &clock_;

    orcaice::Context context_;
};

}

#endif
