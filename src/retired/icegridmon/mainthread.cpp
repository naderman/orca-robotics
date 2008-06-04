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
#include <orcaicegrid/icegridsession.h>

#include "mainthread.h"
#include "registryobserverI.h"
#include "applicationobserverI.h"
#include "adapterobserverI.h"
#include "objectobserverI.h"
#include "nodeobserverI.h"

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
    gbxiceutilacfr::stopAndJoin( iceGridSession_ );
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

    // Create all observers
    registryObserver_ = new RegistryObserverI( context_ );
    IceGrid::RegistryObserverPrx regObserverPrx = 
        orcaice::createConsumerInterface<IceGrid::RegistryObserverPrx> ( context_, registryObserver_ );

    applicationObserver_ = new ApplicationObserverI( context_ );
    IceGrid::ApplicationObserverPrx appObserverPrx = 
        orcaice::createConsumerInterface<IceGrid::ApplicationObserverPrx> ( context_, applicationObserver_ );

    adapterObserver_ = new AdapterObserverI( context_ );
    IceGrid::AdapterObserverPrx adapterObserverPrx = 
        orcaice::createConsumerInterface<IceGrid::AdapterObserverPrx> ( context_, adapterObserver_ );

    objectObserver_ = new ObjectObserverI( context_ );
    IceGrid::ObjectObserverPrx objectObserverPrx = 
        orcaice::createConsumerInterface<IceGrid::ObjectObserverPrx> ( context_, objectObserver_ );

    nodeObserver_ = new NodeObserverI( context_ );
    IceGrid::NodeObserverPrx nodeObserverPrx = 
        orcaice::createConsumerInterface<IceGrid::NodeObserverPrx> ( context_, nodeObserver_ );    

    // create and start session manager
    iceGridSession_ = new orcaicegrid::IceGridSession( context_,
            regObserverPrx, nodeObserverPrx, appObserverPrx, adapterObserverPrx, objectObserverPrx );
    iceGridSession_->start();
    
    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
