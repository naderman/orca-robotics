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

namespace robot2d
{

// Note: this thing self-destructs when run() returns.
//class NetHandler : public orcaice::Thread, public NetFsm
class NetHandler : public orcaice::Thread
{
public:

    NetHandler( orcaice::Buffer<orca::Odometry2dData>& odometryPipe,
                 orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                 const orcaice::Context& context );
    virtual ~NetHandler();

    // from Thread
    virtual void run();

private:

    // network/hardware interface
    orcaice::Buffer<orca::Odometry2dData>& odometryPipe_;
    orcaice::Notify<orca::VelocityControl2dData>& commandPipe_;

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
