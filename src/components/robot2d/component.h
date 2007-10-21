/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_COMPONENT_H
#define ORCA2_ROBOT2D_COMPONENT_H

#include <orcaice/component.h>

// data pipes
#include <hydroutil/proxy.h>
#include <hydroutil/notify.h>
// internal data structures
#include "types.h"
#include <orcarobotdriverutil/hwdriverhandler.h>
#include "hwdriver.h"

namespace robot2d
{

class Component : public orcaice::Component
{
public:

    Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    orca::VehicleDescription loadDriver();

    // loop responsible for interaction with the network
    hydroutil::ThreadPtr netHandler_;
    // loop responsible for interaction with local hardware
    hydroutil::ThreadPtr hwHandler_;

    std::auto_ptr<HwDriver> driver_;
};

} // namespace

#endif
