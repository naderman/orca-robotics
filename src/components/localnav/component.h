/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOCALNAV_COMPONENT_H
#define ORCA2_LOCALNAV_COMPONENT_H

#include <orcaice/component.h>
#include <hydrodll/dynamicload.h>
#include <orcalocalnav/pathfollower2dI.h>
#include <orcalocalnav/clock.h>

namespace localnav {

class MainThread;
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

    MainThread            *mainLoop_;

    // A factory to instantiate the driver
    DriverFactory   *driverFactory_;
    // And the library that provides it
    hydrodll::DynamicallyLoadedLibrary *driverLib_;

    // The global time
    orcalocalnav::Clock *clock_;

    // External interface
    orcalocalnav::PathFollower2dI *pathFollowerInterface_;
};

}

#endif
