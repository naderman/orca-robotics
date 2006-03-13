/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaice/exceptions.h>

#include "networkhandler.h"
#include "displayhandler.h"

using namespace std;
using namespace orca;
using namespace teleop;
using orcaice::operator<<;

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

void NetworkHandler::setupConfigs( const Ice::PropertiesPtr & properties )
{

}

void NetworkHandler::run()
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
    int timeoutMs = (int)floor(1000.0 * orcaice::getPropertyAsDoubleWithDefault(
            context_.properties(), "Teleop.Config.RepeatInterval", 0.2 ) );

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

    while ( isActive() )
    {
        int ret = commandBuffer_->getAndPopNext( command, timeoutMs );

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
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
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
        context_.tracer()->error( "unexpected std exception.");
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

    cout<<"NetworkHandler: stopped."<<endl;
}
