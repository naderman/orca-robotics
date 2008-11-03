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
#include <orca/laserscanner2d.h>
#include <orcaifacestring/laserscanner2d.h>
#include <orcaice/orcaice.h>

#include "mainsubsystem.h"

using namespace std;
using namespace lasermon;

MainSubsystem::MainSubsystem( const orcaice::Context &context ) : 
    Subsystem( context, "MainSubsystem" )
{
}

void
MainSubsystem::initialise()
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    //
    // REQUIRED INTERFACE: Laser
    //
    orca::LaserScanner2dPrx laserPrx;
    // Connect directly to the interface (multi-try)
    orcaice::connectToInterfaceWithTag<orca::LaserScanner2dPrx>( 
        context_, laserPrx, "LaserScanner2d", this, subsysName() );
    
    // Try to get laser description once, continue if fail
    context_.tracer().info( "Trying to get laser description" );
    try
    {
        std::string descr = ifacestring::toString( laserPrx->getDescription() );
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
            // NOTE: we receive the data as a pointer to a generic RangeScanner2dData
            // But we know that we connected to a Laser2d interface which actually sends out
            // LaserScanner2dData objects. So in order to use all the information (e.g. intensities)
            // we need to upcast the object to the derived class.
            // We could print it out 'as is' and the toString() function would slice it (treat it as
            // the base class) -- this is exactly what will happen when we start receiving data
            // through the subscription.
            orca::RangeScanner2dDataPtr rangeDataPtr = laserPrx->getData();
            orca::LaserScanner2dDataPtr laserDataPtr = orca::LaserScanner2dDataPtr::dynamicCast( rangeDataPtr );
            if ( laserDataPtr ) {
                context_.tracer().info( "got experimental laser scan" );
                context_.tracer().print( ifacestring::toString( laserDataPtr ) );
            }
            else {
                context_.tracer().error( "got range scan but failed to convert it to laser scan" );
                // NOTE: this should never happen because above we insisted on connecting specifically
                // to a LaserScanner2dPrx not some generic RangeScanner2dPrx.
                // May as well shutdown now.
                context_.shutdown();
            }
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
    
    // subscribe for data updates (multi-try)
    consumer_ = new orcaifaceimpl::PrintingRangeScanner2dConsumerImpl( context_, 1000, 1 );
    consumer_->subscribeWithTag( "LaserScanner2d", this, subsysName() );
    
    // we won't be updating heartbeat after this
    subStatus().setMaxHeartbeatInterval( -1 );
}
