/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_NETWORK_Thread_H
#define ORCA2_ROBOT2D_NETWORK_Thread_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydroutil/notify.h>

#include <orcaifaceimpl/odometry2dImpl.h>
#include <orcaifaceimpl/velocitycontrol2dImpl.h>
#include "hwthread.h"
#include "types.h"

namespace robot2d
{

//
// Handles network interface
//
class NetThread : public hydroutil::SafeThread,
                  public hydroutil::NotifyHandler<orca::VelocityControl2dData>
{
public:

    NetThread( HwThread                      &HwThread,
                const orca::VehicleDescription &descr,
                const orcaice::Context         &context );
    virtual ~NetThread();

    // from SafeThread
    virtual void walk();

    // from NotifyHandler
    virtual void handleData(const orca::VelocityControl2dData& obj);

private:

    void limit( Command &cmd );

    // external interfaces
    orcaifaceimpl::Odometry2dImplPtr           odometry2dI_;
    orcaifaceimpl::VelocityControl2dImplPtr    velocityControl2dI_;

    HwThread &HwThread_;

    orca::VehicleDescription descr_;

    double maxSpeed_;
    double maxTurnrate_;

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
