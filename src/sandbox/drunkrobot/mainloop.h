/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRUNK_ROBOT_MAIN_LOOP_H
#define ORCA2_DRUNK_ROBOT_MAIN_LOOP_H


#include <hydroutil/thread.h>
#include <hydroutil/buffer.h>

#include <orca/position2d.h>

namespace drunk 
{

// Implements the main loop of our drunken robot.
// It's very convenient to inherit from hydroutil::Thread
// as it means really we only have to implement one method:
// run().


class MainLoop: public hydroutil::Thread 
{
public: 

    // The posData pointer tells us where to put the data
    // The Consumer is a proxy for IceStorm, we will use that to pass data to 
    // Ice storm. 
    MainLoop(hydroutil::Buffer<orca::Position2dData> &posData,
                       const orca::Position2dConsumerPrx &position2dConsumer);

    ~MainLoop(); 

    virtual void run();

private: 
    hydroutil::Buffer<orca::Position2dData>    & posBuffer_;

    orca::Position2dConsumerPrx position2dConsumer_;
};

} // namespace

#endif
