/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRUNK_ROBOT_COMPONENT_H
#define ORCA2_DRUNK_ROBOT_COMPONENT_H

#include <orcaice/component.h> 
#include <orca/position2d.h>

#include <hydroutil/buffer.h>

namespace drunk 
{

class MainLoop; 

class Component : public orcaice::Component
{
public:
    Component();
    ~Component();
    
    virtual void start();
    virtual void stop();

private:

    MainLoop *mainLoop_;

    // Now the buffers that go to the ICE interface:
    hydroutil::Buffer<orca::Position2dData> posBuffer_; 

    orca::Position2dGeometry geom_; 

    // This is used to set up our interface 
    Ice::ObjectPtr position2dObj_;

    // Finally, we also define a proxy to our IceStorm channel
    orca::Position2dConsumerPrx position2dConsumer_;

};

} // namespace

#endif
