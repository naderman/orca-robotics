/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_INPUT_THREAD_H
#define ORCA2_TELEOP_INPUT_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydrointerfaces/humaninput2d.h>
#include <hydrodll/dynamicload.h>

namespace teleop
{

class Network;
// class InputDriver;

class InputThread : public hydroutil::SafeThread
{
public:

    InputThread( Network* network, const orcaice::Context& context );
    virtual ~InputThread();

    // from SafeThread
    virtual void walk();

private:

    Network* network_;

    // Generic driver for the hardware
    hydrointerfaces::HumanInput2d *driver_;
    // A factory to instantiate the driver
    hydrointerfaces::HumanInput2dFactory *driverFactory_;
    // And the library that provides it
    hydrodll::DynamicallyLoadedLibrary *driverLib_;

    // component current context
    orcaice::Context context_;

};

} // namespace

#endif
