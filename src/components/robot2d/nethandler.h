/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_NETWORK_HANDLER_H
#define ORCA2_ROBOT2D_NETWORK_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/proxy.h>
#include <orcaice/notify.h>

#include <orcaifaceimpl/odometry2diface.h>
#include <orcaifaceimpl/velocitycontrol2diface.h>

#include "types.h"

namespace robot2d
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread,
                   public orcaice::NotifyHandler<orca::VelocityControl2dData>
{
public:

    NetHandler( orcaice::Proxy<Data>&          dataPipe,
                orcaice::Notify<Command>&       commandPipe,
                const orca::VehicleDescription& descr,
                const orcaice::Context&         context );
    virtual ~NetHandler();

    // from Thread
    virtual void run();

    // from NotifyHandler
    virtual void handleData(const orca::VelocityControl2dData& obj);

private:

    void activate();
    void initOdom2d();
    void initVelocityControl2d();

    // external interfaces
    orcaifaceimpl::Odometry2dIfacePtr           odometry2dI_;
    orcaifaceimpl::VelocityControl2dIfacePtr    velocityControl2dI_;

    // hardware->network data flow
    orcaice::Proxy<Data>& dataPipe_;
    // network->hardware command flow
    orcaice::Notify<Command>& commandPipe_;

    orca::VehicleDescription descr_;

    // component current context
    orcaice::Context context_;

    // utilities
    static void convert( const robot2d::Data& internal, orca::Odometry2dData& network );
    static void convert( const orca::VelocityControl2dData& network, robot2d::Command& internal );
};

} // namespace

#endif
