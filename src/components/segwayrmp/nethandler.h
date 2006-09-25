/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RMP_NETWORK_HANDLER_H
#define ORCA2_RMP_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>
#include <orcaice/timer.h>

//#include "netfsm.h"

#include <orca/platform2d.h>
#include <orca/position3d.h>
#include <orca/power.h>

namespace segwayrmp
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::PtrProxy<orca::Position2dDataPtr>     & position2dPipe,
                 orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe,
                 orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const orcaice::Context                        & context );
    virtual ~NetHandler();

    // from Thread
    virtual void run();

    // from HwFsm
    void send();
//     virtual void sleep();
//     virtual void repair();

private:

    void init();
    
    //
    // EXTERNAL INTERFACES
    //
    orca::Position2dConsumerPrx position2dPublisher_;
    orca::Position3dConsumerPrx position3dPublisher_;
    orca::PowerConsumerPrx powerPublisher_;

    // network/hardware interface
    orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe_;
    orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe_;
    orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe_;
    orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe_;

    // Internal data storage
    orca::Position2dDataPtr position2dData_;
    orca::Position3dDataPtr position3dData_;
    orca::Velocity2dCommandPtr commandData_;
    orca::PowerDataPtr powerData_;

    // component current context
    orcaice::Context context_;

    // timers for publishing to icestorm
    orcaice::Timer position2dPublishTimer_;
    orcaice::Timer position3dPublishTimer_;
    orcaice::Timer powerPublishTimer_;
    orcaice::Timer statusPublishTimer_;

    // publish intervals
    double position2dPublishInterval_;
    double position3dPublishInterval_;
    double powerPublishInterval_;
    double statusPublishInterval_;

    // dodgy states
    int receiveStatus_;
    int sendStatus_;

};

} // namespace

#endif
