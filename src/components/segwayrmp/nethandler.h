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

#include <orcaifaceimpl/odometry2diface.h>
#include <orcaifaceimpl/odometry3diface.h>
#include <orcaifaceimpl/poweriface.h>

#include "types.h"

namespace segwayrmp
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::Proxy<Data>&              dataPipe,
                orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                const orca::VehicleDescription&               descr,
                const orcaice::Context&                       context );
    virtual ~NetHandler();

    // from Thread
    virtual void run();

private:

    void activate();
    void initOdom2d();
    void initOdom3d();
    void initPower();

    // external interfaces
    orcaifaceimpl::Odometry2dIfacePtr odometry2dI_;
    orcaifaceimpl::Odometry3dIfacePtr odometry3dI_;
    orcaifaceimpl::PowerIfacePtr      powerI_;

    // network/hardware interface
    orcaice::Proxy<Data>&              dataPipe_;
    orcaice::Notify<orca::VelocityControl2dData>& commandPipe_;

    orca::VehicleDescription                    descr_;

    // component current context
    orcaice::Context context_;

    // utilities
    static void convert( const Data& internal, orca::Odometry2dData& network );
    static void convert( const Data& internal, orca::Odometry3dData& network );
    static void convert( const Data& internal, orca::PowerData& network );
    static void convert( const orca::VelocityControl2dData& network, segwayrmp::Command& internal );
};

} // namespace

#endif
