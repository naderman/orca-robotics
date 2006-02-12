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

#include "networkhandler.h"

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/exceptions.h>

using namespace std;
using namespace orca;
using orcaiceutil::operator<<;

NetworkHandler::NetworkHandler( orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> *commandBuffer )
    : commandBuffer_(commandBuffer)
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
    // create a null pointer. data will be cloned into it.
    Ice::ObjectPtr data;
    // create and init command to default 'halt' command
    Velocity2dCommandPtr command = new Velocity2dCommand;
    command->motion.v.x = 0.0;
    command->motion.v.y = 0.0;
    command->motion.w = 0.0;

    // configs
    timeoutMs_ = (int)floor(1000.0 * orcaiceutil::getPropertyAsDoubleWithDefault(
            context_.properties(), "Teleop.Config.RepeatInterval", 0.2 ) );

    // REQUIRED : Platform2d
    while ( isActive() ) {
        try
        {
            orcaiceutil::connectToInterfaceWithTag<Platform2dPrx>( context_, platform2dPrx_, "Platform2d" );
            context_.tracer()->print("connected to a 'Platform2d' interface");
            break;
        }
        // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
        catch ( const Ice::LocalException & e )
        {
            context_.tracer()->debug("failed to connect to a remote interface");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    while ( isActive() )
    {
        int ret = commandBuffer_->getAndPopNext( command, timeoutMs_ );

        //
        // Sending motion command (probably over the network)
        //
        try {
            platform2dPrx_->setCommand( command );
                
            if ( ret==0 ) { // new command
                cout<<endl<<command<<endl;
                //cout<<"NEW : <<command<<endl;
            }
            else {
                cout<<"."<<flush;
                //cout<<"old : "<<command<<endl;
            }
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            // note: cannot throw one of our exceptions from here
            // because we are running in our own thread
            // so do nothing, just keep trying (it will check for active next time around)
            cout<<"x"<<flush;
        }
        catch ( const Ice::TimeoutException & e ) {
            cout<<"-"<<flush;
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

    cout<<"NetworkHandler: stopped."<<endl;
}
