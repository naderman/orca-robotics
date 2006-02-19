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
#include <orcaice/ptrbuffer.h>
#include <orcaice/ptrnotify.h>
#include <orcaice/timer.h>

//#include "netfsm.h"

#include <orca/platform2d.h>
#include <orca/power.h>


// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dProxy,
                 orcaice::PtrNotify                             & commandNotify,
                 orcaice::PtrBuffer<orca::PowerDataPtr>         & powerProxy,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer,
                 const orcaice::Context                         & context );
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
    orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dBuffer_;
    orcaice::PtrNotify                             & commandNotify_;
    orcaice::PtrBuffer<orca::PowerDataPtr>         & powerBuffer_;
    orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer_;
    orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer_;

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

#endif
