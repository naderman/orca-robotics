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

#ifndef ORCA2_RMP_MAIN_LOOP_H
#define ORCA2_RMP_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/ptrnotify.h>
#include <orcaice/timer.h>

#include "rmpdriver.h"

#include <orca/platform2d.h>
#include <orca/power.h>


//! Note: this thing self-destructs when run() returns.
class RmpMainLoop : public orcaice::Thread, public orcaice::PtrNotifyHandler
{
public:

    RmpMainLoop( orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dProxy,
                 orcaice::PtrNotify                             & commandNotify,
                 orcaice::PtrBuffer<orca::PowerDataPtr>         & powerProxy,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer,
                 const orca::Position2dConsumerPrx                  & position2dPublisher,
                 const orca::PowerConsumerPrx                       & powerPublisher );
    virtual ~RmpMainLoop();

    void setCurrent( const orcaice::Context & context ) { context_=context; };

    virtual void run();

    virtual void handleData( const Ice::ObjectPtr & obj );

private:

    // network/driver interface
    orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dProxy_;
    orcaice::PtrNotify                             & commandNotify_;
    orcaice::PtrBuffer<orca::PowerDataPtr>         & powerProxy_;
    orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer_;
    orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer_;

    // IceStorm consumers
    orca::Position2dConsumerPrx position2dPublisher_;
    orca::PowerConsumerPrx powerPublisher_;

    // generic interface to the hardware
    RmpDriver* driver_;

    void readConfigs();
    RmpDriver::Config config_;

    RmpDriver::DriverType driverType_;

    // component current context
    orcaice::Context context_;

    // timers for publishing to icestorm
    orcaice::Timer position2dPublishTimer_;
    orcaice::Timer powerPublishTimer_;
    orcaice::Timer statusPublishTimer_;
    
    // debug
    orcaice::Timer readTimer_;
    orcaice::Timer writeTimer_;

};

#endif
