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

#ifndef ORCA2_DR_COMPONENT_H
#define ORCA2_DR_COMPONENT_H

#include <orcaice/component.h> 
#include <orca/position2d.h>

#include <orcaice/orcaice.h>
#include <orcaice/ptrnotify.h>


class DrunkRobotMainLoop; 

class DrunkRobotComponent : public orcaice::Component
{
public:
    DrunkRobotComponent();
    ~DrunkRobotComponent();
    virtual void start();
    virtual void stop();

 private:

    DrunkRobotMainLoop *mainLoop_;

    // Now the buffers that go to the ICE interface:

    orcaice::PtrBuffer<orca::Position2dDataPtr> posBuffer_; 

    orca::Position2dGeometryPtr geom_; 

    // This is used to set up our interface 

    Ice::ObjectPtr position2dObj_;

    // Finally, we also define a proxy to our IceStorm channel

    orca::Position2dConsumerPrx position2dConsumer_;

};

#endif
