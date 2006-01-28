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
            current_.properties(), "Teleop.Config.RepeatTime", 0.2 ) );

    // REQUIRED : Platform2d
    while ( isActive() ) {
        try
        {
            orcaiceutil::connectToInterfaceWithTag<Platform2dPrx>( current_, platform2dPrx_, "Platform2d" );
            current_.tracer()->print("connected to a 'Platform2d' interface");
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            //current_.tracer()->print("failed to connect to a remote interface");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    while ( isActive() )
    {
        int ret = commandBuffer_->getAndPopNext( command, timeoutMs_ );

        if ( ret==0 ) { // new data
            cout<<endl<<command<<endl;
            //cout<<"NEW : <<command<<endl;
        }
        else {
            cout<<"."<<flush;
            //cout<<"old : "<<command<<endl;
        }

        //
        // Sending motion command (probably over the network)
        //
        try {
            platform2dPrx_->setCommand( command );
        }
        catch ( const orca::HardwareFailedException & e ) {
            cout<<e.what<<endl;
        }
        catch ( const Ice::UnknownException & e ) {
            cout<<"Unknown exception from the platform"<<endl;
            cout<<e<<endl;
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            /*  cannot throw from one thread to another :(
            string errorStr = "Lost connection to the platform server.";
            throw orcaiceutil::OrcaIceUtilNetworkException( ERROR_INFO, errorStr );
            */
            // do nothing, just keep trying
        }
        catch ( const Ice::CommunicatorDestroyedException & e )
        {
            // it's ok, the communicator may already be destroyed
            cout<<"Communicator has passed away. No worries."<<endl;
        }

        //int dt = timer.stop();
        // store dt
        //timer.restart();
    }

    cout<<"NetworkHandler: stopped."<<endl;
}
