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

#include <orcaice/configutils.h>
#include <orcaice/sysutils.h>
#include <orcaice/exceptions.h>

#include <orcaobj/stringutils.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "ConfigTest" ) {};
    virtual ~TestComponent() {};

    // component interface
    virtual void start();
    virtual void stop() {};
};

void 
TestComponent::start()
{
    cout<<"testing getProvidedInterface() ... ";
    try {
        orca::FQInterfaceName fqIface = orcaice::getProvidedInterface( context(), "P1" );
        std::string strIface = orcaice::toString( fqIface );
        if ( strIface != "p1@"+orcaice::getHostname()+"/configtest" ) {
            cout<<"failed"<<endl<<"got :"<<strIface<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface P1 should exist"<<endl;
        exit(EXIT_FAILURE);
    }
    
    try {
        orca::FQInterfaceName fqIface = orcaice::getProvidedInterface( context(), "P100" );
        cout<<"failed"<<endl<<"interface P100 should not exist"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getProvidedInterface() with empty tag ... ";
    try {
        orca::FQInterfaceName fqIface = orcaice::getProvidedInterface( context(), "" );
        cout<<"failed"<<endl<<"empty tag should be caught"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }

    cout<<"testing getProvidedTopic() ... ";
    try {
        orca::FQTopicName fqTopic = orcaice::getProvidedTopic( context(), "P1" );
        std::string strTopic = orcaice::toString( fqTopic );
        if ( strTopic != "p1/*@"+orcaice::getHostname()+"/configtest" ) {
            cout<<"failed"<<endl<<"got :"<<strTopic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface P1 should exist"<<endl;
        exit(EXIT_FAILURE);
    }
    
    try {
        orca::FQTopicName fqTopic = orcaice::getProvidedTopic( context(), "P100" );
        cout<<"failed"<<endl<<"interface P100 should not exist"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getProvidedTopic() with empty tag ... ";
    try {
        orca::FQTopicName fqTopic = orcaice::getProvidedTopic( context(), "" );
        cout<<"failed"<<endl<<"empty tag should be caught"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getRequiredInterfaceAsString() ... ";
    try {
        std::string strPrx = orcaice::getRequiredInterfaceAsString( context(), "R1" );
        std::string expect = "r1@remote/random";
        if ( strPrx != expect ) {
            cout<<"failed"<<endl<<"expected: "<<expect<<"; got :"<<strPrx<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface R1 (indirect proxy syntax) should exist"<<endl;
        exit(EXIT_FAILURE);
    }

    try {
        std::string strPrx = orcaice::getRequiredInterfaceAsString( context(), "R2" );
        std::string expect = "r2 -t:tcp -h localhost -p 15000";
        if ( strPrx != expect ) {
            cout<<"failed"<<endl<<"expected: "<<expect<<"; got :"<<strPrx<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface R2 (direct proxy syntax) should exist"<<endl;
        exit(EXIT_FAILURE);
    }
    
    try {
        std::string strPrx = orcaice::getRequiredInterfaceAsString( context(), "R100" );
        cout<<"failed"<<endl<<"interface R100 should not exist"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getRequiredInterfaceAsString() with empty tag ... ";
    try {
        std::string strPrx = orcaice::getRequiredInterfaceAsString( context(), "" );
        cout<<"failed"<<endl<<"empty tag should be caught"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;
    
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
