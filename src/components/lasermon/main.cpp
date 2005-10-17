/*
 *  Orca: Components for robotics.
 *
 *  Copyright (C) 2004-2005
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

#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orca/laser.h>
#include "laserconsumer_i.h"
#include "lasermoninput.h"

#include <orcaiceutil/configutils.h>

using namespace std;
using namespace orca;


class App : virtual public Ice::Application
{
    public:
        virtual int run(int, char * []);
};

int App::run( int argc, char* argv[] )
{
    // create the one-and-only component adapter, parse config file to create adapter name
    orcaiceutil::setComponentProperties( communicator() );

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter( "Orca" );

    // PROVIDED : LaserConsumer
    Ice::ObjectPrx prx = adapter->addWithUUID( new LaserConsumerI );
    // make a direct proxy
    Ice::ObjectPrx callbackPrx = adapter->createDirectProxy( prx->ice_getIdentity() );

    // start processing clients' requests
    adapter->activate();
    cout<<"*** INFO: provided interfaces are initialized and running..."<<endl;

    // REQUIRED : Position2d
    // create a proxy for the remote server based on its name in the config file
    std::string proxyName = orcaiceutil::getRemotePortName( communicator(), "Laser" );

    Ice::ObjectPrx base = communicator()->stringToProxy(proxyName);
    // check with the server that the one we found is of the right type
    Position2dPrx position2dPrx;
    int count = 0;
    while ( true ) 
    {
        try {
            position2dPrx = Position2dPrx::checkedCast(base);
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            if ( count++ > 5 ) exit(0);
            cout<<"tried "<<count<<" times"<<endl;
            sleep(1);
        }
    }

    // Get the IceStorm position2dTopic
    std::string topicName = orcaiceutil::getRemoteTopicName( communicator(), "Laser" );
    IceStorm::TopicPrx position2dTopic = orcaiceutil::getIceStormTopic( communicator(), topicName );
    assert(position2dTopic);

    //
    // HARDWARE INTERFACES
    //
    orcaiceutil::Driver* driver = new Position2dMonInput( position2dPrx, position2dTopic, callbackPrx );
    driver->setup( communicator()->getProperties() );

    // this is a dodgy hack, to let the driver shutdown the communicator and end the program
    Position2dMonInput* hack = (Position2dMonInput*)driver;
    hack->setupCommunicator( communicator() );

    driver->activate();
    
    // Wait until we are done (this will trap signals)
    //
    communicator()->waitForShutdown();

    // do clean up if there was a Ctrl-C, otherwise the driver has cleaned up itself
    if ( interrupted() )  {
        cerr<< appName() << ": terminating..." << endl;
        driver->deactivate();
        cout<<"joining... Hit any key please."<<endl;
        driver->getThreadControl().join();
    } else {
        cout<<appName()<<": exiting cleanly. Good bye."<<endl;
    }

    return 0;
}

int main(int argc, char * argv[])
{
    App app;
    return app.main(argc, argv);
}
