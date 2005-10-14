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

#include "deviceuser_i.h"

#include <orcaiceutil/device.h>
#include <orcaiceutil/objutils.h>

//#include <sstream>

using namespace std;
using namespace orca;

void menu()
{
    cout <<
            "usage:\n"
            "p: push data\t\tClientPush\n"
            "g: get data\t\tClientPull (blocking)\n"
            "r: request data\t\tClientPull (non-blocking)\n"
            "s: subscribe\t\tServerPush\n"
            "u: usibscribe\n"
            "q: shutdown server\n"
            "x: exit\n"
            "?: help\n";
}

class App : virtual public Ice::Application
{
    public:
        virtual int run(int, char * []);
};

int App::run( int argc, char* argv[] )
{
    // create a proxy for the remote server based on it's address in the config file
    Ice::PropertiesPtr properties = communicator()->getProperties();
    const char* proxyProperty = "OrcaComponent.DeviceInterface";
    std::string proxyName = properties->getProperty(proxyProperty);
    if(proxyName.empty()) {
        cerr << appName() << ": property `" << proxyProperty << "' not set" << endl;
        return EXIT_FAILURE;
    }

    cout<<"looking for proxy "<<proxyName<<endl;
    Ice::ObjectPrx base = communicator()->stringToProxy(proxyName);

    // check with the server that the one we found is of the right type
    DevicePrx device;
    int count = 0;
    while ( true )
    {
        try {
            device = DevicePrx::checkedCast(base);
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e ) {
            if ( count++ > 5 ) exit(1);
            cout<<"tried "<<count<<" times"<<endl;
            sleep(1);
        }
    }

    SimpleLaserScan scan;
    orcaiceutil::setSane( scan );

    // endpoints are in the config file!
    //Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("OrcaComponent");
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints(
            "OrcaComponent", "default");

    // create servant
    Ice::ObjectPtr deviceUser = new DeviceUserI;
    // tell adapter about the new servant (just make up a unique name, could also use UUID)
    adapter->add( deviceUser, Ice::stringToIdentity("callback") );

    // make a generic proxy
    Ice::ObjectPrx objectPrx = adapter->createProxy(Ice::stringToIdentity("callback"));

    // this proxy will be passed to the server to call back
    DeviceUserPrx twowayR = DeviceUserPrx::uncheckedCast( objectPrx );

    // start processing clients' requests
    adapter->activate();

    //
    // Get the IceStorm topic
    //
    cout<<"connecting to IceStorm..."<<endl;
    proxyProperty = "IceStorm.TopicManager.Proxy";
    proxyName = properties->getProperty(proxyProperty);
    Ice::ObjectPrx base1 = communicator()->stringToProxy(proxyName);
    IceStorm::TopicManagerPrx stormManager = IceStorm::TopicManagerPrx::checkedCast(base1);
    if(!stormManager) {
        cerr << appName() << ": invalid proxy" << endl;
        return EXIT_FAILURE;
    }
    
    //
    // Retrieve the topic by name
    IceStorm::TopicPrx topic;
    try
    {
        topic = stormManager->retrieve("coffee/laser");
    }
    catch(const IceStorm::NoSuchTopic& e)
    {
        cerr << appName() << ": " << e << " name: " << e.name << endl;
        return EXIT_FAILURE;
    }
    assert(topic);

    menu();

    char c;
    do
    {
        try
        {
            cout << "==> ";
            cin >> c;
            if(c == 'p')
            {
                device->acceptDataPush( scan );
            }
            else if(c == 'r')
            {
                device->requestData( twowayR );
            }
            else if(c == 'g')
            {
                SimpleLaserScan scan = device->getData( twowayR );
                cout<<"Received laser scan ("<<scan.returns.size()<<" returns) :";
                for ( uint i=0; i<scan.returns.size() && i<25; i+=5 ) {
                    cout<<scan.returns[i].range<<":"<<(int)scan.returns[i].intensity<<" ";
                }
                cout<<endl;
            }
            else if(c == 's')
            {
                //device->subscribe( twowayR );

                // Set the requested quality of service "reliability" =
                // "batch". This tells IceStorm to send events to the subscriber
                // in batches at regular intervals.
                IceStorm::QoS qos;
                qos["reliability"] = "batch";

                topic->subscribe(qos, objectPrx);
            }
            else if(c == 'u')
            {
                topic->unsubscribe( objectPrx );
            }
            /*
            else if(c == 'i')
            {
                device->setPushInterval( 100+rand()%10000 );
            } */
            else if(c == 'q')
            {
                device->shutdown();
            }
            else if(c == '?')
            {
                menu();
            }
        }
        catch(const Ice::Exception& ex)
        {
            cerr << ex << endl;
        }
    }
    while(cin.good() && c != 'x');

    return 0;
}

int main(int argc, char * argv[])
{
    App app;
    return app.main(argc, argv);
}
