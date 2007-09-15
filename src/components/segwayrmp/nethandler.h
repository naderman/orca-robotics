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

#include <orcaice/safethread.h>
#include <orcaice/context.h>
#include <orcaice/proxy.h>
#include <orcaice/notify.h>

#include <orcaifaceimpl/odometry2dImpl.h>
#include <orcaifaceimpl/odometry3dImpl.h>
#include <orcaifaceimpl/powerImpl.h>
#include <orcaifaceimpl/velocitycontrol2dImpl.h>
#include <orcarobotdriverutil/hwdriverhandler.h>
#include "types.h"

namespace segwayrmp
{

class NetHandler : public orcaice::SafeThread,
                   public orcaice::NotifyHandler<orca::VelocityControl2dData>
{
public:

    NetHandler( orcarobotdriverutil::HwDriverHandler<Command,Data> &hwDriverHandler,
                const orca::VehicleDescription                     &descr,
                const orcaice::Context                             &context );

    // from SafeThread
    virtual void walk();

    // from NotifyHandler
    virtual void handleData(const orca::VelocityControl2dData &incomingCommand);

private:

    void limit( Command &command );

    // external interfaces
    orcaifaceimpl::Odometry2dImplPtr           odometry2dI_;
    orcaifaceimpl::Odometry3dImplPtr           odometry3dI_;
    orcaifaceimpl::PowerImplPtr                powerI_;
    orcaifaceimpl::VelocityControl2dImplPtr    velocityControl2dI_;

    orcarobotdriverutil::HwDriverHandler<Command,Data> &hwDriverHandler_;

    orca::VehicleDescription descr_;

    double maxSpeed_;
    double maxTurnrate_;

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
