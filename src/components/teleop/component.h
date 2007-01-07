/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_COMPONENT_H
#define ORCA2_TELEOP_COMPONENT_H

#include <orcaice/component.h>

#include <orcaice/buffer.h>
#include <orca/platform2d.h>

namespace teleop
{

class NetworkHandler;
class UserHandler;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    // network-hardware interface
    // the driver will put the latest data into this proxy
    orcaice::Buffer<orca::Velocity2dCommand> commandPipe_;

    // network loop
    NetworkHandler* networkHandler_;

    // hardware
    UserHandler* userHandler_;
};

} // namespace

#endif
