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

#ifndef ORCA2_RMP_NETWORK_HANDLER_H
#define ORCA2_RMP_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>
#include <orcaice/timer.h>

//#include "netfsm.h"

#include <orca/platform2d.h>
#include <orca/power.h>

namespace robot2d
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::PtrProxy<orca::Position2dDataPtr>     & position2dPipe,
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
    orca::PowerConsumerPrx powerPublisher_;

    // network/hardware interface
    orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe_;
    orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe_;
    orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe_;

    // Internal data storage
    orca::Position2dDataPtr position2dData_;
    orca::Velocity2dCommandPtr commandData_;
    orca::PowerDataPtr powerData_;

    // component current context
    orcaice::Context context_;

    // timers for publishing to icestorm
    orcaice::Timer position2dPublishTimer_;
    orcaice::Timer powerPublishTimer_;
    orcaice::Timer statusPublishTimer_;

    // publish intervals
    double position2dPublishInterval_;
    double powerPublishInterval_;
    double statusPublishInterval_;

    // dodgy states
    int receiveStatus_;
    int sendStatus_;

};

} // namespace

#endif
