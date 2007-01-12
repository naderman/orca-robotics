/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOCALNAV_COMPONENT_H
#define ORCA2_LOCALNAV_COMPONENT_H

#include <orca/rangescanner2d.h>
#include <orca/velocitycontrol2d.h>
#include <orca/odometry2d.h>
#include <orca/localise2d.h>
#include <orca/pathfollower2d.h>
#include <orcaice/component.h>
#include <orcaice/ptrproxy.h>
#include <orcaifaceimpl/proxiedconsumerI.h>
#include <orcaifaceimpl/ptrproxiedconsumerI.h>
#include "pathfollower2dI.h"

namespace localnav {

class IDriver;
class MainLoop;
class IDriver;
class LocalNavManager;
class PathMaintainer;
class Simulator;

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
    orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr> *obsConsumer_;
    orcaifaceimpl::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>     *locConsumer_;
    orcaifaceimpl::ProxiedConsumerI<orca::Odometry2dConsumer,orca::Odometry2dData>     *odomConsumer_;

    // Give commands
    orca::VelocityControl2dPrx *velocityControl2dPrx_;

    // Simulator for test mode
    Simulator           *testSimulator_;

    //
    // ALGORITHM INTERFACES
    //
    IDriver         *driver_;
    LocalNavManager *localNavManager_;
    PathMaintainer  *pathMaintainer_;
    MainLoop        *mainLoop_;

    // External interface
    PathFollower2dI                         *pathFollowerInterface_;
};

}

#endif
