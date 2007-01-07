/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RMP_NETWORK_HANDLER_H
#define ORCA2_RMP_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/proxy.h>
#include <orcaice/notify.h>
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

    NetHandler( orcaice::Proxy<orca::Position2dData>     & position2dPipe,
                 orcaice::Proxy<orca::Position3dData>    & position3dPipe,
                 orcaice::Notify<orca::Velocity2dCommand>& commandPipe,
                 orcaice::Proxy<orca::PowerData>         & powerPipe,
                 orcaice::Proxy<orca::Platform2dConfig>  & setConfigPipe,
                 orcaice::Proxy<orca::Platform2dConfig>  & currentConfigPipe,
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
    orcaice::Proxy<orca::Position2dData>    & position2dPipe_;
    orcaice::Proxy<orca::Position3dData>    & position3dPipe_;
    orcaice::Notify<orca::Velocity2dCommand>& commandPipe_;
    orcaice::Proxy<orca::PowerData>         & powerPipe_;
    orcaice::Proxy<orca::Platform2dConfig>  & setConfigPipe_;
    orcaice::Proxy<orca::Platform2dConfig>  & currentConfigPipe_;

    // Internal data storage
    orca::Position2dData position2dData_;
    orca::Position3dData position3dData_;
    orca::Velocity2dCommand commandData_;
    orca::PowerData powerData_;

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

};

} // namespace

#endif
