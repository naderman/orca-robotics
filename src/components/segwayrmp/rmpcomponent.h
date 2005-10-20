/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#include <orcaiceutil/component.h>
#include <orca/platform2d.h>
#include <orca/power.h>

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/ptrnotify.h>

class RmpMainLoop;

class RmpComponent : public orcaiceutil::Component
{
public:

    RmpComponent();
    virtual ~RmpComponent();

    // component interface
    virtual void  start();
    virtual void stop();

private:

    RmpMainLoop* mainLoop_;

    // NETWORK-MAINLOOP INTERFACES
    //
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer<orca::Position2dDataPtr> position2dProxy_;
    // the driver will take the latest command from the proxy
    //orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> commandProxy_;
    orcaiceutil::PtrNotify commandProxy_;
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer<orca::PowerDataPtr> powerProxy_;
    // The servant will put config requests here.  We'll use them to re-configure the hardware.
    orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr> setConfigBuffer_;
    // We put the current config here for the servant to serve.
    orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr> currentConfigBuffer_;

    //
    // EXTERNAL INTERFACES
    //
    Ice::ObjectPtr platform2dObj_;
    orca::Position2dConsumerPrx position2dConsumer_;

    Ice::ObjectPtr powerObj_;
    orca::PowerConsumerPrx powerConsumer_;
};

#endif
