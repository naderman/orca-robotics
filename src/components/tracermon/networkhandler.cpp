/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "networkhandler.h"
#include "events.h"
#include "tracerconsumerI.h"

using namespace std;
using namespace orca;
using namespace tracermon;

NetworkHandler::NetworkHandler( User* user,
                                const orcaice::Context & context ) :
    user_(user),
    events_(new orcaice::EventQueue),
    context_(context)
{
}

NetworkHandler::~NetworkHandler()
{
    cout<<"NetworkHandler distructor"<<endl;
}

void 
NetworkHandler::setVerbosityLevel( int error, int warn, int info, int debug )
{
    orcaice::EventPtr e = new VerbosityLevelsChangedEvent( error, warn, info, debug );
    events_->add( e );
}

void
NetworkHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // configs
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr props = context_.properties();
    int infoVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"InfoVerbosity", 10 );
    int warnVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"WarningVerbosity", 10 );
    int errorVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"ErrorVerbosity", 10 );
    int debugVerb = orcaice::getPropertyAsIntWithDefault( props, prefix+"DebugVerbosity", 10 );

    // REQUIRED : Tracer
    orca::TracerPrx tracerPrx;
    while ( isActive() ) {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::TracerPrx >( context_, tracerPrx, "Tracer" );
            context_.tracer()->debug("connected to a 'Tracer' interface",5);
            break;
        }
        // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
        catch ( const Ice::LocalException & e )
        {
            context_.tracer()->info("failed to connect to a remote interface. Will try again after 2 seconds.");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    //
    // Subscribe for data
    //
//     Ice::ObjectPtr consumer = this;
    Ice::ObjectPtr consumer = new TracerConsumerI;
    orca::TracerConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::TracerConsumerPrx>( context_, consumer );

    while ( isActive() ) {
        try
        {
            tracerPrx->setVerbosity( errorVerb, warnVerb, infoVerb, debugVerb );
            tracerPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & )
        {
            context_.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        catch ( const Ice::Exception & )
        {
            context_.tracer()->error( "Ice exception. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    orcaice::EventPtr event;
    int timeoutMs = 500;

    //
    // Main loop
    //
    while ( isActive() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            continue;
        }

        switch ( event->type() )
        {
        // approx in order of call frequency
        case VerbosityLevelsChanged : {
//             cout<<"verb event"<<endl;
            VerbosityLevelsChangedEventPtr e = VerbosityLevelsChangedEventPtr::dynamicCast( event );
            setRemoteVerbosity();
            break;
        }
        default : {
            cout<<"unknown network event "<<event->type()<<". Ignoring..."<<endl;
        }
        } // switch
    } // end of main loop

    cout<<"NetworkHandler: exited main loop"<<endl;

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        stringstream ss;
        ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        stringstream ss;
        ss << "unexpected std exception, possibly orcaice: " << e.what();
        context_.tracer()->error( ss.str() );
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
    context_.tracer()->debug( "NetworkHandler: stopped.",5 );
}

void 
NetworkHandler::setRemoteVerbosity()
{
//         try {
// //             tracerPrx->setVerbosity( command );
//         }
//         catch ( const Ice::ConnectionRefusedException & e ) {
//             // note: cannot throw one of our exceptions from here
//             // because we are running in our own thread
//             // so do nothing, just keep trying (it will check for active next time around)
//             // todo: should probably try to reconnect instead
// //             displayHandler_->displayEvent( DisplayHandler::FailedToSendCommand );
//             //cout<<"!"<<flush;
//         }
//         catch ( const Ice::TimeoutException & e ) {
// //             displayHandler_->displayEvent( DisplayHandler::FailedToSendCommand );
//             //cout<<"x"<<flush;
//             // keep trying
//         }
//         catch ( const orca::HardwareFailedException & e ) {
//             std::stringstream ss;
//             ss << "networkhandler.cpp::run: Caught HardwareFailedException: " << e.what << endl; 
//             context_.tracer()->warning( ss.str() );
//             // keep trying
//         }
//         catch ( const Ice::UnknownException & e ) {
//             std::stringstream ss;
//             ss << "networkhandler.cpp::run: Caught UnknownException: " << e << endl; 
//             context_.tracer()->warning( ss.str() );
//             // keep trying
//         }
//         catch ( const Ice::CommunicatorDestroyedException & e )
//         {
//             // it's ok, we are probably shutting down
//             cout<<"Communicator has passed away. No worries."<<endl;
//         }
}

// void 
// NetworkHandler::setData(const orca::TracerData& data, const Ice::Current&)
// {
//     cout<<"NetworkHandler::setData "<<endl;
//     EventPtr e = new NewTraceMessageEvent( data );
//     
//     otherQueue_->add( e );
//}
