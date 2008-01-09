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
#include "rangescanner2dconsumerI.h"

using namespace std;
using namespace lasermon;

MainThread::MainThread( const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

void
MainThread::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    //
    // REQUIRED INTERFACE: Laser
    //
    orca::LaserScanner2dPrx laserPrx;
    // Connect directly to the interface
    while ( !isStopping() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::LaserScanner2dPrx>( context_, laserPrx, "LaserScanner2d" );
            break;
        }
        catch ( const orcaice::NetworkException &e )
        {
            std::stringstream ss;
            ss << "Failed to connect to remote object: "<<e.what()<<endl<<"Will try again after 3 seconds.";
            context_.tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }
    
    // Get laser description
    try
    {
        std::string descr = orcaice::toString( laserPrx->getDescription() );
        context_.tracer().info( "Got laser description:\n"+descr );
    }
    catch ( const Ice::Exception & e ) 
    {
        std::stringstream ss;
        ss << "Failed to get laser description. Will continue anyway."<<e;
        context_.tracer().warning( ss.str() );
    }

    // Get laser data once
    int count = 5;
    context_.tracer().info( "Trying to get one scan as a test" );
    while ( count-- )
    {
        try
        {
            context_.tracer().print( orcaice::toString( laserPrx->getData() ) );
            break;
        }
        catch ( const orca::DataNotExistException &e )
        {
            stringstream ss; ss << "Got exception while getting a scan: " << e.what;
            context_.tracer().warning( ss.str() );
        }
        catch ( const orca::HardwareFailedException &e )
        {
            stringstream ss; ss << "Got exception while getting a scan: " << e.what;
            context_.tracer().warning( ss.str() );
        }
    }
    
    // create a callback object to recieve scans
    consumer_ = new RangeScanner2dConsumerI;
    orca::RangeScanner2dConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer_ );
    
    //
    // Subscribe for data
    //
    while ( !isStopping() )
    {
        try
        {
            laserPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException &e )
        {
            stringstream ss;
            ss << "failed to subscribe for data updates: "<<e.what<<endl<<"Will try again after 3 seconds.";
            context_.tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
