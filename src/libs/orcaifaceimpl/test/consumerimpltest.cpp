#include <iostream>
#include <orcaice/component.h>
#include <orcaice/application.h>
#include <orcaice/orcaice.h>
#include <orcaifaceimpl/storingconsumers.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() :
        orcaice::Component( "Test", orcaice::NoStandardInterfaces ) {}
    // component interface
    virtual void start();
    virtual void stop() {};
};

void
TestComponent::start()
{
    // Try creating and destroying the interface a few times
    for ( int i=0; i < 2; i++ )
    {
        cout<<"TRACE(consumerimpltest.cpp): Creating Interface" << endl;        
        orcaifaceimpl::StoringGpsConsumerImplPtr consumer = 
            new orcaifaceimpl::StoringGpsConsumerImpl( context() );
        cout<<"TRACE(consumerimpltest.cpp): Destroying Interface" << endl;
        consumer = 0;
    }
    
    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    cout<<"TRACE(ifacetest.cpp): test PASSED" << endl;
    exit(EXIT_SUCCESS);
}

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
