/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/proxiedconsumerI.h>
#include <orcaice/ptrproxiedconsumerI.h>
#include <orca/rangescanner2d.h>
#include <orca/platform2d.h>
#include <orca/localise2d.h>
#include <orca/pathfollower2d.h>
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
    orcaice::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr> *obsConsumer_;
    orcaice::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>     *locConsumer_;
    orcaice::ProxiedConsumerI<orca::Position2dConsumer,orca::Position2dData>     *odomConsumer_;

    // Give commands
    orca::Platform2dPrx *platform2dPrx_;

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
