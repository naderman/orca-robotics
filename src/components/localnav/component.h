/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/bufferedconsumerI.h>
#include <orcaice/ptrbufferedconsumerI.h>
#include <orca/rangescanner2d.h>
#include <orca/platform2d.h>
#include <orca/localise2d.h>
#include <orca/pathfollower2d.h>
#include "goalwatcher.h"

namespace localnav {

class LocalNavDriver;
class MainLoop;
class LocalNavDriver;
class LocalNavManager;
class PathMaintainer;

//
// Deriving from orcaice::Component means we have to implement start() and stop()
//
class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL INTERFACE
    //

    // The guy to whom we'll publish
    orca::PathFollower2dConsumerPrx pathPublisher_;

    // Get observations, pose, and odometric velocity
    orcaice::PtrBufferedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr> *obsConsumer_;
    orcaice::BufferedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>     *locConsumer_;
    orcaice::BufferedConsumerI<orca::Position2dConsumer,orca::Position2dData>     *odomConsumer_;

    // Give commands
    orca::Platform2dPrx *platform2dPrx_;

    //
    // ALGORITHM INTERFACES
    //
    LocalNavDriver  *localNavDriver_;
    LocalNavManager *localNavManager_;
    PathMaintainer  *pathMaintainer_;
    MainLoop        *mainLoop_;

    //
    // Pipes for talking to the PathFollower2d Object
    //

    // The path we're currently following.  
    // PathFollower2dI puts data in when it arrives, and gets data out when queried by the world.
    orcaice::Proxy<orca::PathFollower2dData> pathPipe_;

    // Used by PathFollower2dI to inform of new paths from the world
    orcaice::Proxy<bool>                           newPathArrivedPipe_;

    // Used by the PathFollowerI to inform of activation commands
    orcaice::Proxy<orca::Time>                     activationPipe_;
    
    // Used by mainLoop to inform of changes in wpIndex
    orcaice::Proxy<int>                            wpIndexPipe_;

    // Allow external en/dis-able
    orcaice::Proxy<bool>                           enabledPipe_;

    // Shared between the driver and the manager
    GoalWatcher      sharedGoalWatcher_;
};

}

#endif
