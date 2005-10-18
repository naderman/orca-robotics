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

#include <orcaiceutil/configutils.h>
#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

NetworkLoop::NetworkLoop( orcaiceutil::PtrBuffer* commandBuffer ) :
        commandBuffer_(commandBuffer)
{
}

NetworkLoop::~NetworkLoop()
{
}

void NetworkLoop::setupComms( const Ice::CommunicatorPtr & communicator )
{
    // REQUIRED : Platform2d
    // create a proxy for the remote server based on its name in the config file
    //! @todo how to get comp. tag in here?
    std::string proxyName = orcaiceutil::getRemotePortName( communicator, "Platform2d", "Teleop" );

    // check with the server that the one we found is of the right type
    int count = 0;
    while ( true ) {
        try {
            platform2dPrx_ = Platform2dPrx::checkedCast( communicator->stringToProxy(proxyName) );
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            if ( count++ > 5 ) exit(1);
            cout<<"tried "<<count<<" times"<<endl;
            sleep(1);
        }
    }
    cout<<"connected"<<endl;
}

void NetworkLoop::setupConfigs( const Ice::PropertiesPtr & properties )
{
    // read update frequency, for example
    timeoutMs_ = 100;
}

void NetworkLoop::run()
{
    cout<<"NetworkLoop::run: starting up..."<<endl;

    // create a null pointer. data will be cloned into it.
    Ice::ObjectPtr data;
    // create and init command to default 'halt' command
    Velocity2dCommandPtr command = new Velocity2dCommand;
    command->motion.v.x = 0.0;
    command->motion.v.y = 0.0;
    command->motion.w = 0.0;

    while ( isActive() )
    {
        int ret = commandBuffer_->getAndPopNext( data, timeoutMs_ );

        if ( ret==0 ) { // new data
            command = Velocity2dCommandPtr::dynamicCast( data );
            cout<<"NEW : "<<command<<endl;
        }
        else {
            cout<<"old : "<<command<<endl;
        }

        try {
            platform2dPrx_->putData( command );
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            cout<<"lost connection. stopping..."<<endl;
            stop();
        }
        //catch ( const Ice::CommunicatorDestroyedException & e ) {
            // it's ok, if this is a Ctrl-C event, the communicator is already destroyed
        //}
    }

    cout<<"NetworkLoop: stopped."<<endl;
}
