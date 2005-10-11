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

#include "outputdriver.h"

#include <orcaiceutil/configutils.h>
#include <orcaiceutil/sane_i.h>

using namespace std;
using namespace orca;
using namespace orca::util;

OutputDriver::OutputDriver( orca::util::PtrProxy* commands ) :
        commandProxy_(commands)
{
}

OutputDriver::~OutputDriver()
{
}

void OutputDriver::setup( const Ice::PropertiesPtr & properties )
{
}

int OutputDriver::setupComms( const Ice::CommunicatorPtr & communicator )
{
    // REQUIRED : Platform2d
    // create a proxy for the remote server based on its name in the config file
    std::string proxyName = orca::util::getRemotePortName( communicator, "Platform2d" );
    if(proxyName.empty()) {
        cerr << "*** ERROR: port name not specified" << endl;
        return EXIT_FAILURE;
    }
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
        catch ( const Ice::Exception & e ) {
            cout<<"Something bad :"<<e<<endl;
        }
    }
    cout<<"connected"<<endl;

    return 0;
}

void OutputDriver::activate()
{
    cout<<"activating device"<<endl;
    start();
}

void OutputDriver::deactivate()
{
    cout<<"deactivating device"<<endl;
    stop();
}

void OutputDriver::run()
{
    // create a null pointer. data will be cloned into it.
    Ice::ObjectPtr data;
    // create and init command to default 'halt' command
    Velocity2dDataPtr command = new Velocity2dData;
    command->speed = 0.0;
    command->sidespeed = 0.0;
    command->turnrate = 0.0;

    // how often to send repeat commands (should be in the config file)
    const int timeoutMs = 100;

    while ( isActive() )
    {
        int ret = commandProxy_->getNextWait( data, timeoutMs );

        if ( ret==0 ) { // new data
            command = Velocity2dDataPtr::dynamicCast( data );
            cout<<"new : "<<command<<endl;
        }

        try {
            platform2dPrx_->putData( command );
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            cout<<"lost connection. quitting..."<<endl;
            deactivate();
        }
        catch ( const Ice::CommunicatorDestroyedException & e ) {
            // it's ok, if this is a Ctrl-C event, the communicator is already destroyed
        }
    }
}
