/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "mainthread.h"

using namespace std;
using namespace brick;

MainThread::MainThread( const orcaice::Context& context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( 20.0 );

    // multi-try function
    orcaice::activate( context_, this, subsysName() );
}

void
MainThread::work()
{
    //
    // Read configuration settings
    //
    std::string prefix = context_.tag() + ".Config.";

    int sleepIntervalMs = orcaice::getPropertyAsIntWithDefault( context_.properties(),
            prefix+"SleepIntervalMs", 1000 );

    setMaxHeartbeatInterval( sleepIntervalMs * 3.0/1000.0 );

    while( !isStopping() )
    {
        context_.tracer().debug( "Running main loop", 5 );
        health().heartbeat();

        // here we can do something useful

        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(sleepIntervalMs));
    }
}
