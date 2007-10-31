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

#include <orcaice/orcaice.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "AppPropTest" ) {};
    virtual ~TestComponent() {};

    // component interface
    virtual void start();
    virtual void stop() {};
};

void TestComponent::start()
{
    cout<<"testing properties in Application ... ";
    int got = properties()->getPropertyAsInt( "Orca.Tracer.ErrorToFile" );
    int expect = properties()->getPropertyAsInt( tag()+".Expect" );
    if ( got != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<got<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    // debug
    cout<<"expect="<<expect<<"; got="<<got<<endl;

    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    exit(EXIT_SUCCESS);
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
