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

#include "initthread.h"

using namespace std;
using namespace pingserver;

InitThread::InitThread( const PingerIPtr& obj, 
                const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "InitThread" ),
    obj_(obj),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

void
InitThread::walk()
{
    // should implement a multitry version
//     obj_->initInterface( this, subsysName() );
    obj_->initInterface();

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
