/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRUNK_ROBOT_MAIN_LOOP_H
#define ORCA2_DRUNK_ROBOT_MAIN_LOOP_H


#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>

#include <orca/position2d.h>

namespace drunk 
{

// Implements the main loop of our drunken robot.
// It's very convenient to inherit from orcaice::Thread
// as it means really we only have to implement one method:
// run().


class MainLoop: public orcaice::Thread 
{
public: 

    // The posData pointer tells us where to put the data
    // The Consumer is a proxy for IceStorm, we will use that to pass data to 
    // Ice storm. 
    MainLoop(orcaice::PtrBuffer<orca::Position2dDataPtr> &posData,
                       const orca::Position2dConsumerPrx &position2dConsumer);

    ~MainLoop(); 

    virtual void run();

private: 
    orcaice::PtrBuffer<orca::Position2dDataPtr>    & posBuffer_;

    orca::Position2dConsumerPrx position2dConsumer_;
};

} // namespace

#endif
