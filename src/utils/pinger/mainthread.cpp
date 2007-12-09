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
#include "latencypingerI.h"

using namespace std;

namespace pinger {

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

    //
    // REQUIRED : Replier
    //
    orca::util::LatencyReplierPrx replierPrx;
    orcaice::connectToInterfaceWithTag<orca::util::LatencyReplierPrx>
        ( context_, replierPrx, "Replier", this );

    //
    // Ping settings
    //
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    LatencyPingerI::Config config;
    // Stop  after  sending  'count' objects.
    config.count = props->getPropertyAsIntWithDefault( prefix+"Count", 101 );
    // Wait 'interval' seconds between sending each packet [ms]
    config.interval = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"Interval", 0.250 );
    // If  preload  is specified, ping sends that many packets not waiting for reply.
    config.preload = props->getPropertyAsIntWithDefault( prefix+"Preload", 1 );
    config.filename = props->getPropertyWithDefault( prefix+"LogFileName", "orcapingresults.dat" );

    //
    // CALLBACK: Pinger
    //
    // (create after connecting to Replier, need its proxy)
    Ice::ObjectPtr pingerObj = new LatencyPingerI( context_, replierPrx, config );
    orca::util::LatencyPingerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::util::LatencyPingerPrx>( context_, pingerObj );

    LatencyPingerIPtr pinger = LatencyPingerIPtr::dynamicCast( pingerObj );

    // register callback proxy with Replier
    replierPrx->setPinger( callbackPrx );

    string driverName = context_.properties()->getPropertyWithDefault( prefix+"Driver", "sequence" );
    string operationType = context_.properties()->getPropertyWithDefault( prefix+"OperationType", "twoway" );
    
    if ( driverName == "sequence" ) 
    {
        if ( operationType == "twoway" )
            pinger->sequencePing( 2 );
        else if ( operationType == "callback" )
            pinger->sequencePing( 1 );
        else  {
            std::string errString = "Unknown operation type: "+operationType;
            context_.tracer()->error( errString );
            throw hydroutil::Exception( ERROR_INFO, errString );
            return;
        }
    }
    else if ( driverName == "iceping" ) 
    {
        pinger->icePing();
    }
    else 
    {
        std::string errString = "Unknown ping method: "+driverName;
        context_.tracer()->error( errString );
        throw hydroutil::Exception( ERROR_INFO, errString );
        return;
    }
    
    context_.communicator()->shutdown();
}

} // namespace
