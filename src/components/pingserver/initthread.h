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
#include <orcaice/context.h>
#include "pingerI.h"

namespace pingserver
{

class InitThread : public hydroiceutil::SubsystemThread
{

public:

    InitThread( const PingerIPtr& obj, 
                const orcaice::Context &context );

    // from SubsystemThread
    virtual void walk();

private:
    PingerIPtr obj_;
    orcaice::Context context_;
};

} // namespace

#endif
