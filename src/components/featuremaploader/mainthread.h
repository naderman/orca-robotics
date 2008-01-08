/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <hydroiceutil/subsystemthread.h>
#include <orcaifaceimpl/featuremap2dImpl.h>
#include <orcaice/context.h>

namespace featuremaploader
{

class MainThread : public hydroiceutil::SubsystemThread
{

public:

    MainThread( const orcaifaceimpl::FeatureMap2dImplPtr& obj, 
                const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:
    orcaifaceimpl::FeatureMap2dImplPtr obj_;
    orcaice::Context context_;
};

} // namespace

#endif
