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
#include <orcaice/orcaice.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "TestTag", orcaice::NoStandardInterfaces ) {};
    virtual void start();
};

void 
TestComponent::start()
{
    cout<<"=========== TESTS =========="<<endl;
    string prefix = context().tag() + ".Config.";
    string comp = context().properties()->getProperty( prefix+"ExpCompName" );
    string platf = context().properties()->getProperty( prefix+"ExpPlatfName" ); 

    // always check platform name.
    // if it specified, that the expected value matches what we have.
    // if it is NOT specified, check that substitution happened and we get something which is not 'local'
    if ( !platf.empty() ) {
        cout<<"testing explicit platform name...";
        if ( context().name().platform != platf ) {
            cout<<"failed: wrong platform name="<<context().name().platform<<" expect="<<platf<<endl;
            exit(EXIT_FAILURE);
        }
        cout<<"ok"<<endl;
    }
    else {
        cout<<"testing implicit or local platform name...";
        if ( context().name().platform == "local" ) {
            cout<<"failed: wrong platform name="<<context().name().platform<<" expect [hostame]"<<endl;
            exit(EXIT_FAILURE);
        }
        cout<<"ok"<<endl;
    }

    // test component name only if the value specified (and is non-empty)
    if ( !comp.empty() ) {
        cout<<"testing component name...";
        if ( context().name().component != comp ) {
            cout<<"failed: wrong component name="<<context().name().component<<" expect="<<comp<<endl;
            exit(EXIT_FAILURE);
        }
        cout<<"ok"<<endl;
    }
    else {
        cout<<"NOT testing component name"<<endl;
    }

    cout<<"=========== TESTS =========="<<endl;
    context().shutdown();
}

int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
