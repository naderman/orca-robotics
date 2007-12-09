/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "mainthread.h"
#include "latencyreplierI.h"

using namespace std;

namespace replier {

MainThread::MainThread( const orcaice::Context& context ) : 
    SafeThread( context.tracer(), context.status() ),
    context_(context)
{
}

MainThread::~MainThread()
{
}

void
MainThread::walk()
{
    orcaice::activate( context_, this );

    // create a provided interface
    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr replierObj = new LatencyReplierI;
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, replierObj, "Replier", this );

    //
    // Main loop
    //   
    waitForStop();
}

} // namespace
