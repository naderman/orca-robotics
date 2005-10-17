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
#include <orcaiceutil/objutils.h>

#include "util.h"

using namespace std;
using namespace orca;
//using namespace orcaiceutil;
using orcaiceutil::operator<<;

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

    adapter->activate();
    cout<<"*** INFO: Adapter is initialized and running..."<<endl;

    // Set up a consumer to receive IceStorm data stream
//     LaserConsumerPtr laserConsumer = new LaserConsumerI;
//     lasermonutil::subscribeConsumer<LaserConsumerPtr>( communicator(), adapter, laserConsumer, "Laser" );

    // create a proxy for the remote server based on its name in the config file
    std::string proxyName = orcaiceutil::getRemotePortName( communicator(), "Laser" );

    Ice::ObjectPrx base = communicator()->stringToProxy(proxyName);
    // check with the server that the one we found is of the right type
    LaserPrx laserPrx;
    int count = 0;
    while ( true ) 
    {
        try {
            laserPrx = LaserPrx::checkedCast(base);
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            if ( count++ > 5 ) exit(0);
            cout<<"tried "<<count<<" times"<<endl;
            sleep(1);
        }
    }


    // We could set the laser's configuration like so:
    //LaserConfigPtr config = new LaserConfig;
    //laserPrx->setConfig( config );

    cout<<"TRACE(main.cpp): " << laserPrx->getConfig() << endl;
    cout<<"TRACE(main.cpp): " << laserPrx->getGeometry() << endl;
    
    // Wait until we are done (this will trap signals)
    //
    communicator()->waitForShutdown();

    return 0;
}

int main(int argc, char * argv[])
{
    App app;
    return app.main(argc, argv);
}
