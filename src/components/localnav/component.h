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
#include <orcadynamicload/dynamicload.h>
#include <orcalocalnav/pathfollower2dI.h>
#include <orcalocalnav/clock.h>
// #include <localnavutil/idriver.h>

namespace localnav {

class IDriver;
class MainLoop;
class DriverFactory;
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

    // Simulator for test mode
    Simulator           *testSimulator_;

    MainLoop            *mainLoop_;

    // A factory to instantiate the driver
    DriverFactory   *driverFactory_;
    // And the library that provides it
    orcadynamicload::DynamicallyLoadedLibrary *driverLib_;

    // The global time
    orcalocalnav::Clock *clock_;

    // External interface
    orcalocalnav::PathFollower2dI *pathFollowerInterface_;
};

}

#endif
