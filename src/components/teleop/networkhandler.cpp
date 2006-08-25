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
#include <orcaice/exceptions.h>

#include "networkhandler.h"
#include "displayhandler.h"

using namespace std;
using namespace orca;
using namespace teleop;

NetworkHandler::NetworkHandler( orcaice::PtrBuffer<orca::Velocity2dCommandPtr> *commandBuffer,
                                DisplayHandler* displayHandler, const orcaice::Context & context )
    : commandBuffer_(commandBuffer),
      displayHandler_(displayHandler),
      context_(context)
{
}

NetworkHandler::~NetworkHandler()
{
}

void
NetworkHandler::setupConfigs( const Ice::PropertiesPtr & properties )
{

}

void
NetworkHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    // create and init command to default 'halt' command
    Velocity2dCommandPtr command = new Velocity2dCommand;
    command->motion.v.x = 0.0;
    command->motion.v.y = 0.0;
    command->motion.w = 0.0;

    // configs
    std::string prefix = context_.tag() + ".Config.";
    
    int timeoutMs = (int)floor(1000.0 * orcaice::getPropertyAsDoubleWithDefault(
            context_.properties(), prefix+"RepeatInterval", 0.2 ) );

    // REQUIRED : Platform2d
    orca::Platform2dPrx platform2dPrx;
    while ( isActive() ) {
        try
        {
            orcaice::connectToInterfaceWithTag<Platform2dPrx>( context_, platform2dPrx, "Platform2d" );
            context_.tracer()->debug("connected to a 'Platform2d' interface",5);
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
    // Main loop
    //
    while ( isActive() )
    {
        int ret = commandBuffer_->getAndPopNext( command, timeoutMs );
        orcaice::setToNow( command->timeStamp );

        //
        // Sending motion command (probably over the network)
        //
        try {
            platform2dPrx->setCommand( command );
                
            if ( ret==0 ) { // new command
                //displayHandler_->displayEvent( DisplayHandler::SentNewCommand );
                displayHandler_->displayCommand( command );
                //cout<<endl<<command<<endl;
            }
            else {
                displayHandler_->displayEvent( DisplayHandler::SentRepeatCommand );
                //cout<<"."<<flush;
            }
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            // note: cannot throw one of our exceptions from here
            // because we are running in our own thread
            // so do nothing, just keep trying (it will check for active next time around)
            // todo: should probably try to reconnect instead
            displayHandler_->displayEvent( DisplayHandler::FailedToSendCommand );
            //cout<<"!"<<flush;
        }
        catch ( const Ice::TimeoutException & e ) {
            displayHandler_->displayEvent( DisplayHandler::FailedToSendCommand );
            //cout<<"x"<<flush;
            // keep trying
        }
        catch ( const orca::HardwareFailedException & e ) {
            cout<<e.what<<endl;
            // keep trying
        }
        catch ( const Ice::UnknownException & e ) {
            cout<<"Unknown exception from the platform"<<endl;
            cout<<e<<endl;
            // keep trying
        }
        catch ( const Ice::CommunicatorDestroyedException & e )
        {
            // it's ok, we are probably shutting down
            cout<<"Communicator has passed away. No worries."<<endl;
        }

        //int dt = timer.stop();
        // store dt
        //timer.restart();
    }

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
    catch ( const orcaice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected (local?) orcaice exception: " << e.what();
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
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        stringstream ss;
        ss << "unexpected std exception: " << e.what();
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
