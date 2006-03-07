/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_SEGWAY_RMP_COMPONENT_H
#define ORCA2_SEGWAY_RMP_COMPONENT_H

#include <orcaice/component.h>

// external interface definitions
#include <orca/platform2d.h>
#include <orca/power.h>

// data pipes
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>

namespace segwayrmp
{

class HwHandler;
class NetHandler;

class RmpComponent : public orcaice::Component
{
public:

    RmpComponent();
    virtual ~RmpComponent();

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
    // hardware->network
    orcaice::PtrProxy<orca::PowerDataPtr> powerPipe_;
    // hardware->network
    orcaice::PtrProxy<orca::Platform2dConfigPtr> currentConfigPipe_;

    // network->hardware
    orcaice::PtrNotify<orca::Velocity2dCommandPtr> commandPipe_;
    // network->hardware
    orcaice::PtrProxy<orca::Platform2dConfigPtr> setConfigPipe_;
};

} // namespace

#endif
