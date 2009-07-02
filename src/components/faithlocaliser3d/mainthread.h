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
#include <orcaice/context.h>
#include <orcaifaceimpl/localise3d.h>
#include <orcaifaceimpl/odometry3d.h>

namespace faithlocaliser3d
{

class MainThread : public orcaice::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );

private:
    
    // from SubsystemThread
    virtual void initialise();
    virtual void work();
    
    double stdDevPosition_;
    double stdDevOrientation_;
    double minInterPublishPeriodSec_;
    
    orcaifaceimpl::StoringOdometry3dConsumerImplPtr odometry3dConsumer_;
    orcaifaceimpl::Localise3dImplPtr                localiseInterface_;
    
    void initNetworkInterface();

    orcaice::Context context_;
};

} // namespace

#endif
