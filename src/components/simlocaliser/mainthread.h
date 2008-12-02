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

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/localise2d.h>

namespace simlocaliser
{

class Driver;

class MainThread : public orcaice::SubsystemThread
{

public:
    MainThread( const orcaice::Context& context );

private:
    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    orcaifaceimpl::Localise2dImplPtr localise2dImpl_;

    // generic interface to the hardware
    Driver* driver_;
    
    orcaice::Context context_;
};

} // namespace

#endif
