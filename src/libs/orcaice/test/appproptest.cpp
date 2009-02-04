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

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "AppPropTest", orcaice::NoStandardInterfaces ) {};
    virtual void start();
};

void TestComponent::start()
{
    cout<<"testing properties in Application ... ";

    int got = context().properties()->getPropertyAsInt( "Orca.Tracer.ErrorToFile" );

    string prefix = context().tag()+".Config.";
    int expect = context().properties()->getPropertyAsInt( prefix+"Expect" );

    if ( got != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<got<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    // debug
    cout<<"expect="<<expect<<"; got="<<got<<endl;

    context().shutdown();
}

int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
