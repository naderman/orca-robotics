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

#ifndef ORCA2_TELEOP_COMPONENT_H
#define ORCA2_TELEOP_COMPONENT_H

#include <orcaiceutil/component.h>

#include <orcaiceutil/ptrbuffer.h>
#include <orca/platform2d.h>

class NetworkLoop;
class InputLoop;

class TeleopComponent : public orcaiceutil::Component
{
public:

    TeleopComponent();
    virtual ~TeleopComponent();

    // component interface
    virtual void start();
    virtual void stop();

private:
    // network-hardware interface
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> commandProxy_;

    // network loop
    NetworkLoop* networkLoop_;

    // hardware
    InputLoop* inputLoop_;
};

#endif
