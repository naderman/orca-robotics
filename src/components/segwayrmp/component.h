/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_COMPONENT_H
#define ORCA2_SEGWAY_RMP_COMPONENT_H

#include <orcaice/component.h>

// external interface definitions
#include <orca/platform2d.h>
#include <orca/position3d.h>
#include <orca/power.h>

// data pipes
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>

namespace segwayrmp
{

class HwHandler;
class NetHandler;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void  start();
    virtual void stop();

private:
    // loop responsible for interaction with the network
    NetHandler     *netHandler_;
    // loop responsible for interaction with local hardware
    HwHandler      *hwHandler_;

    //
    // INTERFACES BETWEEN NETWORK AND HARDWARE HANDLERS
    //
    // hardware->network
    orcaice::PtrProxy<orca::Position2dDataPtr> position2dPipe_;
    orcaice::PtrProxy<orca::Position3dDataPtr> position3dPipe_;
    // hardware->network
    orcaice::Proxy<orca::PowerData> powerPipe_;
    // hardware->network
    orcaice::PtrProxy<orca::Platform2dConfigPtr> currentConfigPipe_;

    // network->hardware
    orcaice::PtrNotify<orca::Velocity2dCommandPtr> commandPipe_;
    // network->hardware
    orcaice::PtrProxy<orca::Platform2dConfigPtr> setConfigPipe_;
};

} // namespace

#endif
