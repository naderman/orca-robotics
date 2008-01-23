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
#include "sessioncreationcallback.h"

using namespace std;
using namespace icegridmon;

MainThread::MainThread( const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

MainThread::~MainThread()
{
    hydroiceutil::stopAndJoin( sessionManager_ );
}

void
MainThread::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    if ( isStopping() )
        return;

    callback_.reset( new SessionCreationCallback( context_ ) );

    sessionManager_ = new orcaicegrid::SessionManager( *callback_.get(), context_ );
    sessionManager_->start();
    
    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
