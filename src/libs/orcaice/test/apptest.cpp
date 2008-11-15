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
    TestComponent() : orcaice::Component( "Test" ) {};

    // component interface
    virtual void start();
    virtual void stop() {};
};

void 
TestComponent::start()
{
    cout<<"testing component interface settings ... ";
    {
        Ice::PropertiesPtr props = context().properties();
        // ON implicitly
        bool enabled;
        enabled = props->getPropertyAsInt( "Orca.Component.EnableHome" );
        if ( enabled != true ) {
            cout<<"failed"<<endl<<"expected Home to be enabled"<<endl;
            exit(EXIT_FAILURE);
        }
        // ON explicitly
        enabled = props->getPropertyAsInt( "Orca.Component.EnableStatus" );
        if ( enabled != true ) {
            cout<<"failed"<<endl<<"expected Status to be enabled"<<endl;
            exit(EXIT_FAILURE);
        }
        // OFF explicitly
        enabled = props->getPropertyAsInt( "Orca.Component.EnableTracer" );
        if ( enabled != false ) {
            cout<<"failed"<<endl<<"expected Tracer to be disabled"<<endl;
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
int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
