/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "mainthread.h"
#include <sys/prctl.h>

using namespace std;
using namespace pingserver;

MainThread::MainThread( const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    //
    // Allow core dumps even if we're setuid root.
    //
    const int dummy=0;
    prctl(PR_SET_DUMPABLE,1,dummy,dummy,dummy);
}

void
MainThread::walk()
{
    try {
        pingerInterface_ = new PingerI( "Pinger", context_ );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Failed to activate Pinger: " << e.what();
        subStatus().fault( ss.str() );
        context_.shutdown();
    }
    // should implement a multitry version
//     pingerInterface_->initInterface( this, subsysName() );
    pingerInterface_->initInterface();

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
