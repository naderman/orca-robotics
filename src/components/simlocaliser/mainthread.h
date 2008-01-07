/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include <hydroiceutil/safethread.h>
#include <orcaice/context.h>

namespace simlocaliser
{

class Driver;

class MainThread : public hydroiceutil::SafeThread
{

public:
    MainThread( const orcaice::Context& context );
    virtual ~MainThread();

    // from hydroiceutil::SafeThread
    virtual void walk();

private:
    // generic interface to the hardware
    Driver* driver_;
    
    orcaice::Context context_;
};

} // namespace

#endif
