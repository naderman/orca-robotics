/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/configutils.h>
#include <orcaice/stringutils.h>
#include <hydroutil/sysutils.h>
#include <orcaice/exceptions.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "ConfigTest", orcaice::NoStandardInterfaces ) {};
    virtual void start();
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

    cout<<"testing getRequiredInterfaceAsString() with existing tag, indirect local ... ";
    try {
        std::string strPrx = orcaice::getRequiredInterfaceAsString( context(), "R0" );
        std::string expect = "r0@local/random";
        if ( strPrx != expect ) {
            cout<<"failed"<<endl<<"expected: "<<expect<<"; got :"<<strPrx<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( const orcaice::ConfigFileException & ) {
        cout<<"failed"<<endl<<"interface R0 (indirect proxy syntax) should exist"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing getRequiredInterfaceAsString() with existing tag, indirect non-local ... ";
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
        int expect = 5;
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

    cout<<"testing resolveLocalPlatform() with fqname ... ";
    try {
        orca::FQComponentName in, out, expect;
        in.platform = "local";
        in.component = "random";
        out = orcaice::resolveLocalPlatform( context(), in );
        expect.platform = hydroutil::getHostname();
        expect.component = in.component;
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expected: "<<orcaice::toString(expect)<<"; got :"<<orcaice::toString(out)<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( ... ) {
        cout<<"failed, caught something"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing resolveLocalPlatform() with string ... ";
    try {
        std::string strPrx = orcaice::resolveLocalPlatform( context(), "r0@local/random" );
        std::string expect = "r0@"+hydroutil::getHostname()+"/random";
        if ( strPrx != expect ) {
            cout<<"failed"<<endl<<"expected: "<<expect<<"; got :"<<strPrx<<endl;
            exit(EXIT_FAILURE);
        }
    }
    catch ( ... ) {
        cout<<"failed, caught something"<<endl;
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
