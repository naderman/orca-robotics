/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

#include <orcaifaceimpl/localise2d.h>
#include <orcaifaceimpl/gps.h>

namespace gps2localise2d
{

class Driver;

class MainThread : public orcaice::SubsystemThread
{
public:

    MainThread( const orcaice::Context &context );
    ~MainThread();

private:

    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    void subscribeToGps();
    orca::GpsDescription getGpsDescription();
    void initDriver( orca::VehicleDescription vehicleDescr );
    
    // generic algorithm
    Driver* driver_;
    
    // Our external interface
    orcaifaceimpl::Localise2dImplPtr localiseInterface_;

    // Consumers
    orcaifaceimpl::StoringGpsConsumerImplPtr gpsConsumer_;

    orcaice::Context context_;
};

} // namespace

#endif
