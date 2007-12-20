/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include "component.h"
#include <orcaice/orcaice.h>
#include <iostream>

// implementations of Ice objects
#include "rangescanner2dconsumerI.h"

using namespace std;
using namespace lasermon;

Component::Component() : 
    orcaice::Component( "LaserMon"  )
{
}

void
Component::start()
{
    //
    // ENABLE NETWORK
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();

    //
    // REQUIRED INTERFACE: Laser
    //
    orca::LaserScanner2dPrx laserPrx;
    // Connect directly to the interface
    // will try forever (to be able Ctrl-C, use a separate thread)
    while ( true )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::LaserScanner2dPrx>( context(), laserPrx, "LaserScanner2d" );
            break;
        }
        catch ( const orcaice::NetworkException &e )
        {
            std::stringstream ss;
            ss << "Failed to connect to remote object: "<<e.what()<<endl<<"Will try again after 3 seconds.";
            tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }
    
    // Get laser description
    try
    {
        std::string descr = orcaice::toString( laserPrx->getDescription() );
        tracer().info( "Got laser description:\n"+descr );
    }
    catch ( const Ice::Exception & e ) 
    {
        std::stringstream ss;
        ss << "Failed to get laser description. Will continue anyway."<<e;
        tracer().warning( ss.str() );
    }

    // Get laser data once
    int count = 5;
    tracer().info( "Trying to get one scan as a test" );
    while ( count-- )
    {
        try
        {
            tracer().print( orcaice::toString( laserPrx->getData() ) );
            break;
        }
        catch ( const orca::DataNotExistException &e )
        {
            stringstream ss; ss << "Got exception while getting a scan: " << e.what;
            tracer().warning( ss.str() );
        }
        catch ( const orca::HardwareFailedException &e )
        {
            stringstream ss; ss << "Got exception while getting a scan: " << e.what;
            tracer().warning( ss.str() );
        }
    }
    
    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new RangeScanner2dConsumerI;
    orca::RangeScanner2dConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context(), consumer );
    
    //
    // Subscribe for data
    //
    // will try forever (to be able Ctrl-C, use a separate thread)
    while ( true )
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
            tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
    // nothing to do
}
