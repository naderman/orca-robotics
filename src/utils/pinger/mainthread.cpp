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

#include "handler.h"
#include "pingerI.h"

using namespace std;

namespace pinger {

Handler::Handler( const orcaice::Context& context ) : 
    context_(context)
{
}

Handler::~Handler()
{
}

void
Handler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
        // Not sure if actually need to make sure that the regisry is reachable here.
        // can probably just activate and then possibly loop when connecting to the req. iface.
        // Just keep trying till this works...
        while ( !isStopping() )
        {
            try {
                context_.activate();
                break;
            }
            catch ( orcaice::NetworkException & e )
            {
                cout<<e.what()<<endl;
            }
            catch ( Ice::Exception & e )
            {
                cout<<e<<endl;
            }
            catch ( ... )
            {
                cout << "Caught some other exception while activating." << endl;
            }
    
            context_.tracer()->debug( "Failed to activate. Will try again...", 3 );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }

        //
        // REQUIRED : Replier
        //
        orca::ReplierPrx replierPrx;
        while ( !isStopping() ) {
            try
            {
                orcaice::connectToInterfaceWithTag<orca::ReplierPrx>( context_, replierPrx, "Replier" );
                context_.tracer()->debug("connected to a 'Replier' interface",5);
                break;
            }
            // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
            catch ( const Ice::LocalException & )
            {
                context_.tracer()->info("Failed to connect to a remote interface. Will try again after 2 seconds.");
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
            }
        }

        //
        // Ping settings
        //
        Ice::PropertiesPtr props = context_.properties();
        std::string prefix = context_.tag() + ".Config.";
        ping_config_t config;
        // Stop  after  sending  'count' objects.
        config.count = props->getPropertyAsIntWithDefault( prefix+"Count", 101 );
        // Wait 'interval' seconds between sending each packet [ms]
        config.interval = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"Interval", 0.250 );
        // If  preload  is specified, ping sends that many packets not waiting for reply.
        config.preload = props->getPropertyAsIntWithDefault( prefix+"Preload", 1 );
    
        //
        // logfile
        //
        std::string logFileName  = props->getPropertyWithDefault( prefix+"LogFileName", "orcapingresults.dat" );
        std::ofstream logfile( logFileName.c_str() );

        //
        // CALLBACK: Pinger
        //
        // (create after connecting to Replier, need its proxy)
        Ice::ObjectPtr pingerObj = new PingerI( context_, replierPrx, &logfile, config );
        orca::PingerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::PingerPrx>( context_, pingerObj );

        PingerIPtr pinger = PingerIPtr::dynamicCast( pingerObj );

        // register callback proxy with Replier
        replierPrx->setPinger( callbackPrx );

        string driverName = context_.properties()->getPropertyWithDefault( prefix+"Driver", "sequence" );
        int operationType = context_.properties()->getPropertyAsIntWithDefault( prefix+"OperationType", 2 );
        if ( driverName == "sequence" ) {
            pinger->sequencePing( operationType );
        }
        else if ( driverName == "class" ) {
            pinger->classPing();
        }
        else if ( driverName == "iceping" ) {
            pinger->icePing();
        }
        else {
            std::string errString = "Unknown ping method: "+driverName;
            context_.tracer()->error( errString );
            throw hydroutil::Exception( ERROR_INFO, errString );
            return;
        }
        
        context_.communicator()->shutdown();

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception (possibly orcaice).");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "handler stopped", 2 );
}

} // namespace
