/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RMP_HARDWARE_HANDLER_H
#define ORCA2_RMP_HARDWARE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/notify.h>
#include <orcaice/proxy.h>
#include <orcaice/timer.h>

#include "hwdriver.h"

#include <orca/platform2d.h>

namespace robot2d
{

// Note: this thing self-destructs when run() returns.
class HwHandler : public orcaice::Thread, public orcaice::NotifyHandler<orca::Velocity2dCommand>
{
public:

    HwHandler( orcaice::Proxy<orca::Position2dData>     & position2dPipe,
               orcaice::Notify<orca::Velocity2dCommand> & commandPipe,
               orcaice::Proxy<orca::Platform2dConfig>   & setConfigPipe,
               orcaice::Proxy<orca::Platform2dConfig>   & currentConfigPipe,
               const orcaice::Context                         & context );
    virtual ~HwHandler();

    // from Thread
    virtual void run();

    // from PtrNotifyHandler
    virtual void handleData( const orca::Velocity2dCommand & obj );

private:

    // network/hardware interface
    orcaice::Proxy<orca::Position2dData>    & position2dPipe_;
    orcaice::Proxy<orca::Platform2dConfig>  & setConfigPipe_;
    orcaice::Proxy<orca::Platform2dConfig>  & currentConfigPipe_;

    // Internal data storage
    orca::Position2dData position2dData_;

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
    orcaice::Proxy<bool> writeStatusPipe_;

    // debug
    orcaice::Timer readTimer_;
    orcaice::Timer writeTimer_;

};

} // namespace

#endif
