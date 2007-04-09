/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_TESTMACHINE_TEST_LOOP_H
#define ORCA2_ROBOT2D_TESTMACHINE_TEST_LOOP_H

#include <Ice/Ice.h>

#include <orcaice/thread.h>
#include <orcaice/context.h>

#include <orca/odometry2d.h>
#include <orca/power.h>


class TestLoop : public orcaice::Thread
{
public:

    TestLoop( const orca::Odometry2dConsumerPrx, const orca::PowerConsumerPrx,
              const orcaice::Context & context );
    ~TestLoop();

    virtual void run();

private:

    void printEvents();
    void connectToPos( orca::Platform2dPrx &platform2dPrx );
    void connectToPower( orca::PowerPrx &powerPrx );

    // Keep track of all significant events
    std::vector<std::string> events_;
    
    // local object to receive data
    orca::Odometry2dConsumerPrx odometry2dCallbackPrx_;
    orca::PowerConsumerPrx powerCallbackPrx_;

    // component current context
    orcaice::Context context_;
};

#endif
