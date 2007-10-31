/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaice/configutils.h>
#include <hydroutil/sysutils.h>
#include <orcaice/exceptions.h>

#include <orcaobj/stringutils.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "ConfigTest", orcaice::NoStandardInterfaces ) {};
    virtual ~TestComponent() {};

    // component interface
    virtual void start();
    virtual void stop() {};
};

void 
TestComponent::start()
{
    cout<<"testing getProvidedInterface() with existing tag ... ";
    try {
        orca::FQInterfaceName fqIface = orcaice::getProvidedInterface( context(), "P1" );
        std::string strIface = orcaice::toString( fqIface );
        if ( strIface != "p1@"+hydroutil::getHostname()+"/configtest" ) {
            cout<<"failed"<<endl<<"got :"<<strIface<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface P1 should exist"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
        
    cout<<"testing getProvidedInterface() with NON-existing tag ... ";
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
    cout<<"ok"<<endl;

    cout<<"testing getProvidedTopicWithString() ... ";
    orca::FQTopicName fqTopic = orcaice::getProvidedTopicWithString( context(), "cool", "secret" );
    std::string strTopic = orcaice::toString( fqTopic );
    if ( strTopic != "cool/secret@"+hydroutil::getHostname()+"/configtest" ) {
        cout<<"failed"<<endl<<"got :"<<strTopic<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getProvidedTopicWithTag() with existing tag ... ";
    try {
        orca::FQTopicName fqTopic = orcaice::getProvidedTopicWithTag( context(), "P1" );
        std::string strTopic = orcaice::toString( fqTopic );
        if ( strTopic != "p1/*@"+hydroutil::getHostname()+"/configtest" ) {
            cout<<"failed"<<endl<<"got :"<<strTopic<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface P1 should exist"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getProvidedTopicWithTag() with NON-existing tag ... ";
    try {
        orca::FQTopicName fqTopic = orcaice::getProvidedTopicWithTag( context(), "P100" );
        cout<<"failed"<<endl<<"interface P100 should not exist"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getProvidedTopicWithTag() with empty tag ... ";
    try {
        orca::FQTopicName fqTopic = orcaice::getProvidedTopicWithTag( context(), "" );
        cout<<"failed"<<endl<<"empty tag should be caught"<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ConfigFileException & ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"testing getRequiredInterfaceAsString() with existing tag, indirect ... ";
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
    cout<<"ok"<<endl;

    cout<<"testing getRequiredInterfaceAsString() with existing tag, direct ... ";
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
    cout<<"ok"<<endl;
    
    cout<<"testing getRequiredInterfaceAsString() with NON-existing tag ... ";
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

    cout<<"testing getProvidedTags() with empty pattern ... ";
    {
        std::vector<std::string> tags = orcaice::getProvidedTags( context() );
        int out = tags.size();
        int expect = 3;
        if ( out != expect ) {
            cout<<"failed, wrong number of tags, expected="<<expect<<"; got="<<out<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getProvidedTags() with pattern ... ";
    {
        std::vector<std::string> tags = orcaice::getProvidedTags( context(), "Special" );
        int out = tags.size();
        int expect = 2;
        if ( out != expect ) {
            cout<<"failed, wrong number of tags, expected="<<expect<<"; got="<<out<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getRequiredTags() with empty pattern ... ";
    {
        std::vector<std::string> tags = orcaice::getRequiredTags( context() );
        int out = tags.size();
        int expect = 4;
        if ( out != expect ) {
            cout<<"failed, wrong number of tags, expected="<<expect<<"; got="<<out<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getRequiredTags() with pattern ... ";
    {
        std::vector<std::string> tags = orcaice::getRequiredTags( context(), "Special" );
        int out = tags.size();
        int expect = 2;
        if ( out != expect ) {
            cout<<"failed, wrong number of tags, expected="<<expect<<"; got="<<out<<endl;
            exit(EXIT_FAILURE);
        }
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
