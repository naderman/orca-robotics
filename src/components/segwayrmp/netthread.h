/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

#include <orcaifaceimpl/odometry2dImpl.h>
#include <orcaifaceimpl/odometry3dImpl.h>
#include <orcaifaceimpl/powerImpl.h>
#include <orcaifaceimpl/velocitycontrol2dImpl.h>
#include <orcaifaceimpl/estopImpl.h>
#include "hwthread.h"

namespace segwayrmp
{

class NetThread : public orcaice::SubsystemThread,
                  public gbxiceutilacfr::NotifyHandler<orca::VelocityControl2dData>
{
public:

    NetThread( HwThread                      &hwMainThread,
                const orca::VehicleDescription &descr,
                const orcaice::Context         &context );

    // from SubsystemThread
    virtual void walk();

    // from NotifyHandler
    // this call originates in orcaifaceimpl::VelocityControl2dImpl
    virtual void handleData(const orca::VelocityControl2dData &incomingCommand);

private:

    void initEStopCallback();

    // external interfaces
    orcaifaceimpl::Odometry2dImplPtr           odometry2dI_;
    orcaifaceimpl::Odometry3dImplPtr           odometry3dI_;
    orcaifaceimpl::PowerImplPtr                powerI_;
    orcaifaceimpl::VelocityControl2dImplPtr    velocityControl2dI_;
    
    // we need this reference to call a couple of get/set functions
    HwThread &hwThread_;

    orca::VehicleDescription descr_;
    bool isEStopEnabled_;

    double maxForwardSpeed_;
    double maxReverseSpeed_;
    double maxTurnrate_;
    double maxLateralAcceleration_;

    // component context
    orcaice::Context context_;
};

} // namespace

#endif
