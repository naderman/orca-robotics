/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef PATHMAINTAINER_H
#define PATHMAINTAINER_H

#include <orca/pathfollower2d.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/context.h>

namespace localnav {

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

    PathMaintainer( orcaice::Proxy<orca::PathFollower2dData> &pathPipe,
                    orcaice::Proxy<bool>                           &newPathArrivedPipe,
                    orcaice::Proxy<orca::Time>                     &activationPipe,
                    orcaice::Proxy<int>                            &wpIndexPipe,
                    const orcaice::Context & context );

    //
    // Functions for the LocalNavManager
    //

    const orca::Waypoint2d &currentWaypoint() const;
    bool  isActive() { return wpIndex_ != -1; }
    void  incrementWpIndex();

    //
    // Functions called by MainLoop, to trigger comms with outside world
    //

    void checkForNewPath( orca::PathFollower2dConsumerPrx &pathConsumer );
    void checkForWpIndexChange( orca::PathFollower2dConsumerPrx &pathConsumer );

    // Return the remaining time (in seconds) before we have to be at the next waypoint
    double secToNextWp() const;
    
private: 

    // How long since path activation
    double secSinceActivation() const;
    
    void informWorldOfNewPath( orca::PathFollower2dConsumerPrx pathConsumer, orca::PathFollower2dData& path );
    void informWorldOfNewWpIndex( orca::PathFollower2dConsumerPrx pathConsumer, int newIndex );

    // Issue warnings if the path is screwy in some way
    void checkPathOut( const orca::PathFollower2dData& pathData );

    // Keep a local copy of the path, so we don't have to mess with the buffer every 
    // time the nav manager wants to look at it.
    orca::PathFollower2dData path_;
    
    // The index of the next waypoint being seeked.
    int wpIndex_;

    // If the index changes, we need to tell the world.
    bool wpIndexChanged_;

    // The timing of the entire path is relative to this
    orca::Time pathStartTime_;

    orcaice::Proxy<orca::PathFollower2dData> &pathPipe_;
    orcaice::Proxy<bool>                           &newPathArrivedPipe_;
    orcaice::Proxy<orca::Time>                     &activationPipe_;
    orcaice::Proxy<int>                            &wpIndexPipe_;

    orcaice::Context context_;
};

}

#endif
