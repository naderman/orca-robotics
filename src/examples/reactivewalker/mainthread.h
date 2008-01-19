/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ALGORITHM_HANDLER_H
#define ALGORITHM_HANDLER_H

#include <hydroiceutil/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/bufferedconsumers.h>
#include <orcaifaceimpl/storingconsumers.h>
#include <orca/velocitycontrol2d.h>

#include "algodriver.h"

namespace reactivewalker
{

class MainThread : public hydroiceutil::SubsystemThread
{

public: 

    MainThread( const orcaice::Context& context );
    virtual ~MainThread();

    // from SubsystemThread
    virtual void walk();

private:

    // utilities    
    void initNetwork();
    void initDriver();

    // generic inerface to the algorithm
    AlgoDriver* driver_;

    // local access to remote interfaces
    orcaifaceimpl::BufferedRangeScanner2dConsumerImplPtr laser_;
    orcaifaceimpl::StoringOdometry2dConsumerImplPtr odometry_;
    // remote command interface
    orca::VelocityControl2dPrx commandPrx_;
    
    orcaice::Context context_;
};

} // namespace

#endif
