#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/bufferedconsumerI.h>
#include <orca/rangescanner.h>
#include <orca/platform2d.h>
#include <orca/localise2d.h>
#include <orca/pathfollower2d.h>
#include "goalwatcher.h"

class LocalNavDriver;
class MainLoop;
class LocalNavDriver;
class LocalNavManager;
class PathMaintainer;

//!
//! Deriving from orcaice::Component means we have to implement start() and stop()
//!
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
    orcaice::BufferedConsumerI<orca::RangeScannerConsumer,orca::RangeScannerDataPtr> *obsConsumer_;
    orcaice::BufferedConsumerI<orca::Localise2dConsumer,orca::Localise2dDataPtr>     *locConsumer_;
    orcaice::BufferedConsumerI<orca::Position2dConsumer,orca::Position2dDataPtr>     *odomConsumer_;

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
    orcaice::PtrProxy<orca::PathFollower2dDataPtr> pathPipe_;

    // Used by PathFollower2dI to inform of new paths from the world
    orcaice::Proxy<bool>                           newPathArrivedPipe_;

    // Used by the PathFollowerI to inform of activation commands
    orcaice::Proxy<orca::Time>                     activationPipe_;
    
    // Used by mainLoop to inform of changes in wpIndex
    orcaice::Proxy<int>                            wpIndexPipe_;

    // Shared between the driver and the manager
    GoalWatcher      sharedGoalWatcher_;
};

#endif
