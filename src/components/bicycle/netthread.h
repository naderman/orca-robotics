/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef NETWORK_THREAD_H
#define NETWORK_THREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <gbxsickacfr/gbxiceutilacfr/notify.h>

#include <orcaifaceimpl/odometry2d.h>
#include <orcaifaceimpl/drivebicycleImpl.h>
#include "hwthread.h"

namespace bicycle
{

//
// Handles network interface
//
class NetThread : public orcaice::SubsystemThread,
                  public gbxiceutilacfr::NotifyHandler<orca::DriveBicycleCommand>
{
public:

    NetThread( HwThread                      &HwThread,
                const orca::VehicleDescription &descr,
                const orcaice::Context         &context );

    // from NotifyHandler
    virtual void handleData( const orca::DriveBicycleCommand& command );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    void limit( hydrointerfaces::Bicycle::Command &cmd );

    // external interfaces
    orcaifaceimpl::Odometry2dImplPtr           odometry2dI_;
    orcaifaceimpl::DriveBicycleImplPtr         driveBicycleI_;

    HwThread &HwThread_;

    // a copy of the current speed
    double currentSpeed_;

    orca::VehicleDescription descr_;

    // redundant but casted description of the control
    IceUtil::Handle<orca::VehicleControlVelocityBicycleDescription>
	controlDescr_;

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
