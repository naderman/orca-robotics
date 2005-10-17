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
#include <orcaiceutil/connectutils.h>
#include <orcaiceutil/mathdefs.h>

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

    //
    // Get the laser's configuration details
    //

    // create a proxy for the remote server based on its name in the config file
    LaserPrx laserPrx;
    orcaiceutil::connectProxy<LaserPrx>( communicator(), adapter, laserPrx, "Laser" );

    // We could set the laser's configuration like so:
//     LaserConfigPtr config = new LaserConfig;
//     config->rangeResolution = 0.01;
//     config->angleIncrement  = DEG2RAD(0.5);
//     config->isEnabled       = true;
//     laserPrx->setConfig( config );

    cout<<"TRACE(main.cpp): " << laserPrx->getConfig() << endl;
    cout<<"TRACE(main.cpp): " << laserPrx->getGeometry() << endl;

    //
    // Now read the laser's data
    //

    // Set up a consumer to receive IceStorm data stream
    LaserConsumerPtr laserConsumer = new LaserConsumerI;
//    orcaiceutil::subscribeConsumerToTopic<LaserConsumerPtr>( communicator(), adapter, laserConsumer, "Laser" );
    orcaiceutil::subscribeConsumerToTopic( communicator(), adapter, (Ice::ObjectPtr&) laserConsumer, "Laser" );


    // We could pull data like so:
//     while (true)
//     {
//         cout << "Pulled: " << laserPrx->getData() << endl;
//         sleep(1);
//     }

    //
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
