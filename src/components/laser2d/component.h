/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_COMPONENT_H
#define ORCA2_LASER2D_COMPONENT_H

#include <orca/laserscanner2d.h>
#include <orcaice/component.h>
#include <orcaifaceimpl/laserscanner2dImpl.h>
#include <hydrodll/dynamicload.h>

namespace laser2d {

class DriverFactory;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    hydroutil::ThreadPtr mainLoop_;

    //
    // External Interface
    //
    orcaifaceimpl::LaserScanner2dImplPtr laserInterface_;

    // A factory to instantiate the driver
    DriverFactory   *driverFactory_;
    // And the library that provides it
    hydrodll::DynamicallyLoadedLibrary *driverLib_;
};

}

#endif
