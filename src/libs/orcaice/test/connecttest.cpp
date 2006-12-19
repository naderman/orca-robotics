/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>

// is this dodgy?
#include "../detail/homeI.h"

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    // don't create standard interfaces, because we'll use Home as a dummy iface.
    TestComponent() : orcaice::Component( "ConnectTest", orcaice::NoStandardInterfaces ) {};
    virtual ~TestComponent() {};

    // component interface
    virtual void start();
    virtual void stop() {};
};

void 
TestComponent::start()
{
    orca::ComponentData compData;
    compData.name = context().name();
    Ice::ObjectPtr homeObj = new orcaice::HomeI( compData, properties()->getPropertiesForPrefix("") );
    
    cout<<"testing createInterfaceWithString() ... ";
    try {
        orcaice::createInterfaceWithString( context(), homeObj, "homeless" );
        // ok
    }
    catch ( const orcaice::Exception & ) {
        cout<<"failed"<<endl<<"should be able to create interface"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


    cout<<"testing createInterfaceWithTag() ... ";
    try {
        orcaice::createInterfaceWithTag( context(), homeObj, "P1" );
        // ok
    }
    catch ( const orcaice::Exception & ) {
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
    catch ( const orcaice::Exception & ) {
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

    
    cout<<"testing activate() ... ";
    try {
        activate();
    }
    catch ( const orcaice::NetworkException & e ) {
        cout<<"failed"<<endl<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


    cout<<"testing connectToInterfaceWithString() ... ";
    orca::HomePrx homePrx;

    // generate indirect proxy appropriate for this host
    orca::FQInterfaceName homelessFqName;
    homelessFqName.platform = orcaice::getHostname();
    homelessFqName.component = "connecttest";
    homelessFqName.iface = "homeless";
    try {
        orcaice::connectToInterfaceWithString<orca::HomePrx>( context(), homePrx, orcaice::toString( homelessFqName ) );
    }
    catch ( const orcaice::NetworkException & e ) {
        cout<<"failed"<<endl<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    
    cout<<"testing connectToInterfaceWithTag() ... ";
    // try to connect R1 to non-existant interface
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "R1" );
        cout<<"failed"<<endl<<"should NOT be able to find this interface"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"should be able to read file"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::NetworkException & ) {
        // ok
    }

    // try to connect R2 (indirect) back to P1
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "R2" );
        // ok
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"should be able to read file"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::NetworkException & e ) {
        cout<<e.what()<<endl;
        cout<<"failed"<<endl<<"should be able to connect"<<endl;
        exit(EXIT_FAILURE);
    }

    // try to connect R3 (direct) back to P1
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "R3" );
        // ok
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"should be able to read file"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::NetworkException & e ) {
        cout<<e.what()<<endl;
        cout<<"failed"<<endl<<"should be able to connect"<<endl;
        exit(EXIT_FAILURE);
    }

    // non-existant tag
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "R100" );
        cout<<"failed"<<endl<<"should NOT be able to find this interface"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    catch ( const orcaice::NetworkException & ) {
        cout<<"failed"<<endl<<"should get ConfigFileException"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


// not sure how to test without relying on the IceStorm being there.
//     cout<<"testing connectToTopicWithString() ... ";
//     orca::HomePrx homePrx;
//     try {
//         orcaice::connectToTopicWithString<orca::HomePrx>( context(), homePrx, "homeless/*@local/connecttest" );
//     }
//     catch ( const orcaice::NetworkException & e ) {
//         cout<<"failed"<<endl<<e.what()<<endl;
//         exit(EXIT_FAILURE);
//     }
//     cout<<"ok"<<endl;

    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
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
    return app.main(argc, argv);
}
