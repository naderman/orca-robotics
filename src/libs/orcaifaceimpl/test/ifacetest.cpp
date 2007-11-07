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
    for ( uint i=0; i < 5; i++ )
    {
        cout<<"TRACE(ifacetest.cpp): Creating interface" << endl;

        orcaifaceimpl::WifiImplPtr wifiIface = new orcaifaceimpl::WifiImpl( context(),
                                                                            "wifi@test/testcomponent" );
        wifiIface->initInterface();

        cout<<"TRACE(ifacetest.cpp): Sending data" << endl;

        orca::WifiData wifiData;
        wifiIface->localSetAndSend( wifiData );

        cout<<"TRACE(ifacetest.cpp): Removing interface" << endl;
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
