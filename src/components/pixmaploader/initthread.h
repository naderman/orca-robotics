/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef INIT_THREAD_H
#define INIT_THREAD_H

#include <hydroiceutil/subsystemthread.h>
#include <orcaifaceimpl/pixmapImpl.h>
#include <orcaice/context.h>

namespace pixmaploader
{

class InitThread : public hydroiceutil::SubsystemThread
{

public:

    InitThread( const orcaifaceimpl::PixMapImplPtr& obj, 
                const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:
    orcaifaceimpl::PixMapImplPtr obj_;
    orcaice::Context context_;
};

} // namespace

#endif
