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
#include <orcaestoputil/estopmonitor.h>
#include <orcarmputil/powerbasemanager.h>
#include "publisherthread.h"
#include <hydrodll/dynamicload.h>

namespace segwayrmp
{

class MainThread : public orcaice::SubsystemThread,
                   public orcarmputil::AggregatorCallback,
                   public orcaifaceimpl::AbstractVelocityControl2dCallback,
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

    // from orcaifaceimpl::AbstractVelocityControl2dCallback
    // this call originates in orcaifaceimpl::VelocityControl2dImpl
    virtual void setCommand( const orca::VelocityControl2dData &incomingCommand );

    // from NotifyHandler<orca::EStopData>
    virtual void handleData( const orca::EStopData &incomingEStopData );

    // from orcarmputil::AggregatorCallback
    void hardwareInitialised( int powerbaseID );
    void receiveData( int                                     powerbaseID,
                      const hydrointerfaces::SegwayRmp::Data &data );

    void instantiateHydroDriver( const std::string &driverLibName );

    // external interfaces
    orcaifaceimpl::Odometry2dImplPtr           odometry2dI_;
    orcaifaceimpl::Odometry3dImplPtr           odometry3dI_;
    orcaifaceimpl::PowerImplPtr                powerI_;
    orcaifaceimpl::VelocityControl2dImplPtr    velocityControl2dI_;

    // required interfaces
    orcaifaceimpl::NotifyingEStopConsumerImplPtr eStopConsumerI_;

    std::auto_ptr<orcaestoputil::EStopMonitor> eStopMonitor_;

    hydrointerfaces::SegwayRmp::Capabilities capabilities_;

    PublisherThread *publisherThread_;
    gbxiceutilacfr::ThreadPtr publisherThreadPtr_;
    
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> hydroDriverLib_;

    std::vector<orcarmputil::PowerbaseManagerPtr> powerbaseManagers_;

    orcaice::Context context_;
};

} // namespace

#endif
