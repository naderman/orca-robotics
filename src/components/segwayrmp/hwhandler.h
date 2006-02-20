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

#ifndef ORCA2_RMP_HARDWARE_HANDLER_H
#define ORCA2_RMP_HARDWARE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>
#include <orcaice/timer.h>

//#include "hwfsm.h"
#include "hwdriver.h"

#include <orca/platform2d.h>
#include <orca/power.h>


// Note: this thing self-destructs when run() returns.
//class HwHandler : public orcaice::Thread, public HwFsm, public orcaice::PtrNotifyHandler
class HwHandler : public orcaice::Thread, public orcaice::PtrNotifyHandler
{
public:

    HwHandler( orcaice::PtrProxy<orca::Position2dDataPtr>     & position2dPipe,
               orcaice::PtrNotify                             & commandPipe,
               orcaice::PtrProxy<orca::PowerDataPtr>          & powerPipe,
               orcaice::PtrProxy<orca::Platform2dConfigPtr>   & setConfigPipe,
               orcaice::PtrProxy<orca::Platform2dConfigPtr>   & currentConfigPipe,
               const orcaice::Context                         & context );
    virtual ~HwHandler();

    // from Thread
    virtual void run();

    // from HwFsm
//     virtual void read();
//     virtual void sleep();
//     virtual void repair();

    // from PtrNotifyHandler
    virtual void handleData( const Ice::ObjectPtr & obj );

private:

    // network/hardware interface
    orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe_;
    orcaice::PtrNotify                            & commandPipe_;
    orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe_;

    // Internal data storage
    orca::Position2dDataPtr position2dData_;
    orca::Velocity2dCommandPtr commandData_;
    orca::PowerDataPtr powerData_;
    // internal RMP status
    HwDriver::Status rmpStatus_;

//     enum RmpState { RMP_INIT, RMP_OK, RMP_READ_FALED, RMP_WRITE_FAILED };

    // generic interface to the hardware
    HwDriver* driver_;

    void init();

    struct Config
    {
        double maxSpeed;
        double maxTurnrate;
    };
    Config config_;

    // component current context
    orcaice::Context context_;

    // dodgy states
    int writeStatus_;

    // debug
    orcaice::Timer readTimer_;
    orcaice::Timer writeTimer_;

};

#endif
