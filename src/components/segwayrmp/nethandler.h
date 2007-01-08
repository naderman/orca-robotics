/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_NETWORK_HANDLER_H
#define ORCA2_SEGWAY_RMP_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/proxy.h>
#include <orcaice/notify.h>
#include <orcaice/timer.h>

#include <orca/odometry2d.h>
#include <orca/velocitycontrol2d.h>
#include <orca/odometry3d.h>
#include <orca/power.h>

namespace segwayrmp
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::Proxy<orca::Odometry2dData>&       odometry2dPipe,
                 orcaice::Proxy<orca::Odometry3dData>&      odometry3dPipe,
                 orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                 orcaice::Proxy<orca::PowerData>&           powerPipe,
                 const orcaice::Context&                    context );
    virtual ~NetHandler();

    // from Thread
    virtual void run();

private:

    // network/hardware interface
    orcaice::Proxy<orca::Odometry2dData>&       odometry2dPipe_;
    orcaice::Proxy<orca::Odometry3dData>&       odometry3dPipe_;
    orcaice::Notify<orca::VelocityControl2dData>& commandPipe_;
    orcaice::Proxy<orca::PowerData>&            powerPipe_;

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
