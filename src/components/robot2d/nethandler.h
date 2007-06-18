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
#include <orcaice/buffer.h>
#include <orcaice/notify.h>

#include <orca/odometry2d.h>
#include <orca/velocitycontrol2d.h>
#include <orca/vehicledescription.h>

#include "types.h"

namespace robot2d
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::Buffer<Data>& dataPipe,
                orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                const orca::VehicleDescription &descr,
                const orcaice::Context& context );
    virtual ~NetHandler();

    // from Thread
    virtual void run();

private:

    // network/hardware interface
    orcaice::Buffer<Data>& dataPipe_;
    orcaice::Notify<orca::VelocityControl2dData>& commandPipe_;

    orca::VehicleDescription descr_;

    // component current context
    orcaice::Context context_;

    // utilities
    static void convert( const robot2d::Data& internal, orca::Odometry2dData& network );
};

} // namespace

#endif
