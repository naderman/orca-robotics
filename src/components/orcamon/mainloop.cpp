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

#include "mainloop.h"
#include <orcaifaceimpl/printingconsumers.h>

using namespace std;
using namespace orcamon;

MainLoop::MainLoop( const orcaice::Context &context ) : 
    SafeThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

MainLoop::~MainLoop()
{
}

void
MainLoop::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );

    //
    // REQUIRED INTERFACE: Generic
    //

    // This may throw ConfigFileException, but we don't catch it.
    // If this happens we may as well quit.
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, "Generic" );
    proxyString = orcaice::resolveLocalPlatform( context_, proxyString );
    Ice::ObjectPrx obj = context_.communicator()->stringToProxy( proxyString );

    // connect to it and get its object ID, aka interface type.
    std::string objId;
    while ( !isStopping() )
    {
        try
        {
            // this returns the most derived interface. see also ice_ids()
            objId = obj->ice_id();
            context_.tracer()->debug( "interface type: "+objId,2 );
            break;
        }
        catch ( const Ice::CommunicatorDestroyedException & e )
        {
            // we must be shutting down, ignore
            break;
        }
        catch( const Ice::LocalException & e )
        {
            context_.tracer()->warning( "Failed to lookup ID for "+proxyString+". Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    // now that we know the interface type, we can create an appropriate consumer and subscribe ourselves
    orcaifaceimpl::ConsumerSubscriber* consumerSubscriber = 0;
    if ( objId=="::orca::LaserScanner2d" || objId=="::orca::RangeScanner2d" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingRangeScanner2dConsumerImpl(context_);
    }
//     else if ( objId=="::orca::Imu" )
//     {
//         consumerSubscriber = new orcaifaceimpl::PrintingImuConsumerImpl(context_);
//     }
    else if ( objId=="::orca::Localise2d" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingLocalise2dConsumerImpl(context_);
    }
    else if ( objId=="::orca::Localise3d" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingLocalise3dConsumerImpl(context_);
    }
    else if ( objId=="::orca::OgMap" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingOgMapConsumerImpl(context_);
    }
//     else if ( objId=="::orca::FeatureMap2d" )
//     {
//         consumerSubscriber = new orcaifaceimpl::PrintingFeatureMap2dConsumerImpl(context_);
//     }
    else if ( objId=="::orca::Odometry2d" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingOdometry2dConsumerImpl(context_);
    }
    else if ( objId=="::orca::Odometry3d" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingOdometry3dConsumerImpl(context_);
    }
//     else if ( objId=="::orca::Status" )
//     {
//         consumerSubscriber = new orcaifaceimpl::PrintingStatusConsumerImpl(context_);
//     }
    else if ( objId=="::orca::Gps" )
    {
        consumerSubscriber = new orcaifaceimpl::PrintingGpsConsumerImpl(context_);
    }
//     else if ( objId=="::orca::GpsMapGrid" )
//     {
//         consumerSubscriber = new orcaifaceimpl::PrintingGpsMapGridConsumerImpl(context_);
//     }
//     else if ( objId=="::orca::GpsTime" )
//     {
//         consumerSubscriber = new orcaifaceimpl::PrintingGpsTimeConsumerImpl(context_);
//     }
    else
    {
        context_.tracer()->error( "Unsupported interface type: "+objId+". Quitting..." );
        context_.communicator()->destroy();
    }

    cout<<"TRACE(main.cpp): Connecting to " << proxyString << endl;
    // multi-try function
    consumerSubscriber->subscribeWithString( proxyString, this );

    // nothing else to do
    waitForStop();
}
