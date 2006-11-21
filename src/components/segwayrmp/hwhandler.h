/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RMP_HARDWARE_HANDLER_H
#define ORCA2_RMP_HARDWARE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>
#include <orcaice/proxy.h>
#include <orcaice/timer.h>

#include "hwdriver.h"

#include <orca/platform2d.h>
#include <orca/position3d.h>
#include <orca/power.h>

namespace segwayrmp
{

// Note: this thing self-destructs when run() returns.
class HwHandler : public orcaice::Thread, public orcaice::NotifyHandler<orca::Velocity2dCommandPtr>
{
public:

    HwHandler( orcaice::PtrProxy<orca::Position2dDataPtr>     & position2dPipe,
               orcaice::PtrProxy<orca::Position3dDataPtr>     & position3dPipe,
               orcaice::PtrNotify<orca::Velocity2dCommandPtr> & commandPipe,
               orcaice::PtrProxy<orca::PowerDataPtr>          & powerPipe,
               orcaice::PtrProxy<orca::Platform2dConfigPtr>   & setConfigPipe,
               orcaice::PtrProxy<orca::Platform2dConfigPtr>   & currentConfigPipe,
               const orcaice::Context                         & context );
    virtual ~HwHandler();

    // from Thread
    virtual void run();

    // from PtrNotifyHandler
    virtual void handleData( const orca::Velocity2dCommandPtr & obj );

private:

    // network/hardware interface
    orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe_;
    orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe_;
    orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe_;
    orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe_;

    // Internal data storage
    orca::Position2dDataPtr position2dData_;
    orca::Position3dDataPtr position3dData_;
    orca::PowerDataPtr powerData_;

    // generic interface to the hardware
    HwDriver* driver_;

    void init();

    struct Config
    {
        bool isMotionEnabled;
        double maxSpeed;
        double maxTurnrate;
    };
    Config config_;

    // component current context
    orcaice::Context context_;

    // write status has to be protected to be accessed from both read and write threads
    // true is good, false is bad.
    orcaice::Proxy<bool> writeStatusPipe_;

    // debug
    orcaice::Timer readTimer_;
    orcaice::Timer writeTimer_;

};

} // namespace

#endif
