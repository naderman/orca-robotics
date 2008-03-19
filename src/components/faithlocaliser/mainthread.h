/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <gbxsickacfr/gbxiceutilacfr/subsystemthread.h>
#include <orcaice/context.h>

namespace faithlocaliser
{

class MainThread : public gbxsickacfr::gbxiceutilacfr::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:
    double stdDevPosition_;
    double stdDevHeading_;

    double minInterPublishPeriodSec_;

    orcaice::Context context_;
};

} // namespace

#endif
