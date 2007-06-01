/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_TESTMACHINE_HANDLER_H
#define ORCA2_ROBOT2D_TESTMACHINE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>

#include <orca/odometry2d.h>
#include <orca/power.h>

namespace robot2dtester
{

class Handler : public orcaice::Thread
{
public:

    Handler( const orca::Odometry2dConsumerPrx, const orca::PowerConsumerPrx,
              const orcaice::Context & context );
    ~Handler();

    virtual void run();

private:

    void connectToOdometry( orca::Odometry2dPrx& odometry2dPrx );
    void connectToPower( orca::PowerPrx& powerPrx );

        // local object to receive data
    orca::Odometry2dConsumerPrx odometry2dCallbackPrx_;
    orca::PowerConsumerPrx powerCallbackPrx_;

    // Keep track of all significant events
    std::vector<std::string> events_;
    void printEvents();

    // component current context
    orcaice::Context context_;
};

} // namespace
#endif
