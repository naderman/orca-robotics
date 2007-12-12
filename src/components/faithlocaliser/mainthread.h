/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_FAITH_LOCALISER_MAIN_LOOP_H
#define ORCA2_FAITH_LOCALISER_MAIN_LOOP_H

#include <hydroutil/subsystemthread.h>
#include <orcaice/context.h>

namespace faithlocaliser
{

class MainThread : public hydroutil::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );
    ~MainThread();

    // from SubsystemThread
    virtual void walk();

private:
    double stdDevPosition_;
    double stdDevHeading_;

    orcaice::Context context_;
};

} // namespace

#endif
