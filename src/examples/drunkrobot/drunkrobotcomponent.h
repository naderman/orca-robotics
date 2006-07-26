/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DR_COMPONENT_H
#define ORCA2_DR_COMPONENT_H

#include <orcaice/component.h> 
#include <orca/position2d.h>

#include <orcaice/ptrbuffer.h>


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
