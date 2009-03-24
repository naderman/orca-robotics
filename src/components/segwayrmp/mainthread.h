/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaifaceimpl/odometry2d.h>
#include <orcaifaceimpl/odometry3d.h>
#include <orcaifaceimpl/power.h>
#include <orcaifaceimpl/velocitycontrol2d.h>
#include <orcaifaceimpl/estop.h>
#include "driverthread/driverthread.h"
#include <hydrodll/dynamicload.h>
#include "publisherthread.h"
#include "estopmonitor.h"

namespace segwayrmp
{

class MainThread : public orcaice::SubsystemThread,
                   public segwayrmpdriverthread::Callback,
                   public gbxiceutilacfr::NotifyHandler<orca::VelocityControl2dData>,
                   public gbxiceutilacfr::NotifyHandler<orca::EStopData>
{
public:

    MainThread( const orcaice::Context &context );
    ~MainThread();

private:

    // from SubsystemThread
    virtual void initialise();
    // this subsystem maintain a work thread after initialisation!
    virtual void finalise();

    // from NotifyHandler<orca::VelocityControl2dData>
    // this call originates in orcaifaceimpl::VelocityControl2dImpl
    virtual void handleData( const orca::VelocityControl2dData &incomingCommand );

    // from NotifyHandler<orca::EStopData>
    virtual void handleData( const orca::EStopData &incomingEStopData );

    // from segwayrmpdriverthread::Callback
    virtual void hardwareInitialised();
    virtual void receiveData( const hydrointerfaces::SegwayRmp::Data &data );

    void instantiateHydroDriver( const std::string &driverLibName );

    // external interfaces
    orcaifaceimpl::Odometry2dImplPtr           odometry2dI_;
    orcaifaceimpl::Odometry3dImplPtr           odometry3dI_;
    orcaifaceimpl::PowerImplPtr                powerI_;
    orcaifaceimpl::VelocityControl2dImplPtr    velocityControl2dI_;

    // required interfaces
    orcaifaceimpl::NotifyingEStopConsumerImplPtr eStopConsumerI_;

    std::auto_ptr<EStopMonitor> eStopMonitor_;

    hydrointerfaces::SegwayRmp::Capabilities capabilities_;

    segwayrmpdriverthread::DriverThread *segwayRmpDriverThread_;
    gbxiceutilacfr::ThreadPtr segwayRmpDriverThreadPtr_;

    PublisherThread *publisherThread_;
    gbxiceutilacfr::ThreadPtr publisherThreadPtr_;
    
    // The library that contains the hydro driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> hydroDriverLib_;
    // Generic hydro driver for the hardware
    std::auto_ptr<hydrointerfaces::SegwayRmp> hydroDriver_;

    orcaice::Context context_;
};

} // namespace

#endif
