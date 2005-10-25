/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include "networkloop.h"

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/exceptions.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

NetworkLoop::NetworkLoop( orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> *commandBuffer )
    : commandBuffer_(commandBuffer)
{
}

NetworkLoop::~NetworkLoop()
{
}

int NetworkLoop::connect()
{
    // REQUIRED : Platform2d
    // create a proxy for the remote server based on its name in the config file
    std::string proxyName = orcaiceutil::getRemotePortName( current_, "Platform2d" );

    // check with the server that the one we found is of the right type
    try {
        platform2dPrx_ = Platform2dPrx::checkedCast( current_.communicator()->stringToProxy(proxyName) );
        return 0;
    }
    catch ( const Ice::ConnectionRefusedException & e ) {
        return 1;
    }
}

void NetworkLoop::setupConfigs( const Ice::PropertiesPtr & properties )
{

}

void NetworkLoop::run()
{
    cout<<"starting networkloop"<<endl;
    //current_.logger()->print("running");

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

    // establish connection
    while ( isActive() && connect() ) {
        //current_.logger()->trace("remote","failed to connect to a remote interface");
        sleep(2);
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

        try {
            platform2dPrx_->setCommand( command );
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

    cout<<"NetworkLoop: stopped."<<endl;
}
