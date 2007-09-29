/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    // if set to FALSE, will return success when registry is not available
    bool failWithoutRegistry = false;

    cout<<"testing activate() ... ";
    try {
        activate();
    }
    catch ( const orcaice::NetworkException & e ) {
        if ( failWithoutRegistry ) {
            cout<<"failed"<<endl<<e.what()<<endl;
            exit(EXIT_FAILURE);
        }
        else {
            cout<<"could not connect to registry:"<<endl<<e.what()<<endl;
            cout<<"The test is configured not to fail."<<endl;
            exit(EXIT_SUCCESS);
        }
    }
    cout<<"ok"<<endl;
    // from now on, we know that the registry exists, so we don't tolerate exceptions

    Ice::ObjectPtr homeObj = new orcaice::HomeI( interfaceFlag(), context() );
    
    cout<<"testing createInterfaceWithString() ... ";
    try {
        orcaice::createInterfaceWithString( context(), homeObj, "homeless" );
        // ok
    }
    catch ( const orcaiceutil::Exception & ) {
        cout<<"failed"<<endl<<"should be able to create interface"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


    cout<<"testing createInterfaceWithTag() ... ";
    try {
        orcaice::createInterfaceWithTag( context(), homeObj, "P1" );
        // ok
    }
    catch ( const orcaiceutil::Exception & ) {
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
    catch ( const orcaiceutil::Exception & ) {
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
    homelessFqName.platform = orcaiceutil::getHostname();
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

    cout<<"testing connectToInterfaceWithTag() to good indirect ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "IndirectGood" );
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
    cout<<"ok"<<endl;
    
    cout<<"testing connectToInterfaceWithTag() to indirect with bad comp name ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "IndirectBadCompName" );
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
    cout<<"ok"<<endl;

    cout<<"testing connectToInterfaceWithTag() to indirect with bad iface name ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "IndirectBadName" );
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
    cout<<"ok"<<endl;

    cout<<"testing connectToInterfaceWithTag() to good direct ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "DirectGood" );
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
    cout<<"ok"<<endl;

    cout<<"testing connectToInterfaceWithTag() to direct with bad name ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "DirectBadName" );
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
    cout<<"ok"<<endl;

    cout<<"testing connectToInterfaceWithTag() to direct with bad endpoint ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "DirectBadEndpoint" );
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
    cout<<"ok"<<endl;

    cout<<"testing connectToInterfaceWithTag() with bad tag ... ";
    try {
        orcaice::connectToInterfaceWithTag<orca::HomePrx>( context(), homePrx, "BadTag" );
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
