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
#include <orcaifaceimpl/featuremap2d.h>
#include <orcaice/context.h>

namespace featuremaploader
{

class MainThread : public orcaice::SubsystemThread
{

public:

    MainThread( const orcaice::Context& context );

private:

    // from SubsystemThread
    virtual void initialise();
    // this subsystem does not work!

    orcaifaceimpl::FeatureMap2dImplPtr featureMap2dImpl_;
    orcaice::Context context_;
};

} // namespace

#endif
