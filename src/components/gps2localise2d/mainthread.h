/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <hydroutil/subsystemthread.h>
#include <orcaice/context.h>

#include <orca/gps.h>
#include <orcaifaceimpl/localise2dImpl.h>
#include <orcaifaceimpl/storingconsumers.h>

namespace gps2localise2d
{

class Driver;

class MainThread : public hydroutil::SubsystemThread
{
public:

    MainThread( const orcaice::Context &context );

    ~MainThread();

    virtual void walk();

private:

    void subscribeToGps();
    void getGpsDescription();
    void initNetworkInterface();
    void initDriver();
    
    // This component is 2D-centric: can only handle certain orientations.
    bool antennaOffsetOK( const orca::Frame3d &antennaOffset );

    // generic algorithm
    Driver* driver_;
    
    // Our external interface
    orcaifaceimpl::Localise2dImplPtr localiseInterface_;
    
    orca::GpsPrx gpsPrx_;
    orca::Frame3d antennaOffset_;
    orca::GpsDescription gpsDescr_;

    // Consumers
    orcaifaceimpl::StoringGpsConsumerImplPtr gpsConsumer_;

    orcaice::Context context_;
};

} // namespace

#endif
