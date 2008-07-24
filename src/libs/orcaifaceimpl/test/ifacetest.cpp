#include <iostream>
#include <orcaice/component.h>
#include <orcaice/application.h>
#include <orcaice/orcaice.h>
#include <orcaifaceimpl/wifiImpl.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent()
        : orcaice::Component( "Test" )
        {}

    // component interface
    virtual void start();
    virtual void stop();
};

void
TestComponent::start()
{
    // Try creating and destroying the interface a few times
    for ( int i=0; i < 5; i++ )
    {
        cout<<"TRACE(ifacetest.cpp): Creating Interface" << endl;
        orcaifaceimpl::WifiImplPtr wifiIface = new orcaifaceimpl::WifiImpl( context(),
                                                                            "wifi@test/testcomponent" );
        try {
            wifiIface->initInterface();
        }
        catch ( const orcaice::NetworkException &e )
        {
            // This can happen if IceStorm isn't running...
            cout<<"TRACE(ifacetest.cpp): While trying to initInterface(), caught: " << e.what() << endl;
            cout<<"TRACE(ifacetest.cpp): This can happen if IceStorm isn't running."<<endl<<"Can't really test unless IceStorm is running, so we're gonna pass the test regardless." << endl;
            exit( EXIT_SUCCESS );
        }

        cout<<"TRACE(ifacetest.cpp): Sending Data" << endl;

        orca::WifiData wifiData;
        wifiIface->localSetAndSend( wifiData );

        cout<<"TRACE(ifacetest.cpp): Removing Interface" << endl;
    }
    
    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    cout<<"TRACE(ifacetest.cpp): test PASSED" << endl;
    exit(EXIT_SUCCESS);
}

void
TestComponent::stop()
{
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
