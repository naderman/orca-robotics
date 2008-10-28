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

using namespace std;

namespace systemstatusmon {

MainThread::MainThread( const orcaice::Context &context ) : 
    SafeThread( context.tracer() ),
    context_(context)
{
}

void
MainThread::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );

    //
    // REQUIRED INTERFACE: SystemStatus
    //
    orca::SystemStatusPrx systemStatusPrx;
    
    // Connect directly to the interface (multi-try)
    orcaice::connectToInterfaceWithTag<orca::SystemStatusPrx>( 
            context_, systemStatusPrx, "SystemStatus", this );
    
    consumer_ = new orcaifaceimpl::PrintingSystemStatusConsumerImpl( context_ );
    consumer_->subscribeWithTag( "SystemStatus", this );
}

}
