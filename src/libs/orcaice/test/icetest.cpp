/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orca/home.h>

using namespace std;

class HomeI : public orca::Home
{
public:
    // remote interface
    virtual orca::HomeData getData(const ::Ice::Current& )
        { return orca::HomeData(); }
};

class TestComponent : public orcaice::Component
{
public:
    // don't create standard interfaces, because we'll use Home as a dummy iface.
    TestComponent() : orcaice::Component( "IceTest", orcaice::NoStandardInterfaces ) {};
    virtual void start();
};

void 
TestComponent::start()
{
    // if set to FALSE, will return success when registry is not available
    const bool failWithoutRegistry = false;

    cout<<"testing isRegistryReachable() ... ";
    {
        bool isUp = orcaice::isRegistryReachable( context() );
        if ( !isUp ) {
            if ( failWithoutRegistry ) {
                cout<<"failed: could not reach registry"<<endl;
                exit(EXIT_FAILURE);
            }
            else {
                cout<<"could not connect to registry."<<endl;
                cout<<"The test is configured not to fail."<<endl;
                exit(EXIT_SUCCESS);
            }
        }
    }
    cout<<"ok"<<endl;

    // from now on, we know that the registry exists, so we don't tolerate exceptions

    cout<<"testing activate() ... ";
    try {
        activate();
    }
    catch ( const orcaice::NetworkException & e ) {
        cout<<"failed"<<endl<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    Ice::ObjectPtr homeObj = new HomeI();

    cout<<"testing createInterfaceWithString() ... ";
    {
        try {
            orcaice::createInterfaceWithString( context(), homeObj, "homeless" );
            // ok
        }
        catch ( const gbxutilacfr::Exception & ) {
            cout<<"failed"<<endl<<"should be able to create interface"<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"testing isInterfaceReachable() with existing interface ... ";
    {
        string proxyString = "homeless@" + orcaice::toString(context().name());
        string diagnostic;
        bool isUp = orcaice::isInterfaceReachable( context(), proxyString, diagnostic );
        if ( !isUp ) {
            cout<<"failed"<<endl<<"should be able to ping interface"<<endl;
            cout<<"diagnostic="<<diagnostic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing createInterfaceWithTag() ... ";
    try {
        orcaice::createInterfaceWithTag( context(), homeObj, "P1" );
        // ok
    }
    catch ( const gbxutilacfr::Exception & ) {
        cout<<"failed"<<endl<<"should be able to create interface"<<endl;
        exit(EXIT_FAILURE);
    }

    // try non-existant interface
    try {
        orcaice::createInterfaceWithTag( context(), homeObj, "P100" );
        cout<<"failed"<<endl<<"should NOT be able to create interface"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    catch ( const gbxutilacfr::Exception & ) {
        cout<<"failed"<<endl<<"should get ConfigFileException"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


    cout<<"testing createConsumerInterface() ... ";
    // here, we make HomeI into a 'consumer' object, it's ok
    // every object can have multiple identities in the adapter
    orca::HomePrx homeDirPrx =
            orcaice::createConsumerInterface<orca::HomePrx>( context(), homeObj );
    homeDirPrx->ice_getIdentity();            
    // it doesn't throw and there's nothing to test, just make sure it works            
    cout<<"ok"<<endl;

    cout<<"testing connectToInterfaceWithString() ... ";
    orca::HomePrx homePrx;

    // generate indirect proxy appropriate for this host
    orca::FQInterfaceName homelessFqName;
    homelessFqName.platform = hydroutil::getHostname();
    homelessFqName.component = context().name().component;
    homelessFqName.iface = "homeless";
    try {
        orcaice::connectToInterfaceWithString( context(), homePrx, orcaice::toString( homelessFqName ) );
    }
    catch ( const orcaice::NetworkException & e ) {
        cout<<"failed"<<endl<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    context().shutdown();
}

int 
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
