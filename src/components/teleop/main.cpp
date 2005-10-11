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

#include <orcaiceutil/ptrproxy.h>

#include "inputdriver.h"
#include "outputdriver.h"

using namespace std;
using namespace orca;

class App : virtual public Ice::Application
{
    public:
        virtual int run(int, char * []);

    private:

        Platform2dPrx platform2dPrx_;
};

int App::run( int argc, char* argv[] )
{
    //
    // NETWORK-DRIVER INTERFACE
    //
    // the driver will put the latest data into this proxy
    orca::util::PtrProxy commandProxy;

    //
    // NETWORK
    //
    OutputDriver* netDriver = new OutputDriver( &commandProxy );
    // dodgy hack: non-standard function
    netDriver->setupComms( communicator() );
    netDriver->activate();

    //
    // DRIVER
    //
    orca::util::Driver* driver = new InputDriver( &commandProxy );
    driver->setup( communicator()->getProperties() );

    // this is a dodgy hack, to let the driver shutdown the communicator and end the program
    InputDriver* hack = (InputDriver*)driver;
    hack->setupCommunicator( communicator() );

    driver->activate();

    // Wait until we are done (this will trap signals)
    //
    communicator()->waitForShutdown();

    // do clean up if there was a Ctrl-C, otherwise the driver has cleaned up itself
    if ( interrupted() )  {
        cerr<< appName() << ": terminating..." << endl;

        netDriver->deactivate();
        cout<<"joining network driver..."<<endl;
        netDriver->getThreadControl().join();

        driver->deactivate();
        cout<<"joining input driver... Hit any key please."<<endl;
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
