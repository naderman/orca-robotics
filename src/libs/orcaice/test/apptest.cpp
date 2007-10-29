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

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent();
    virtual ~TestComponent();

    // component interface
    virtual void start();
    virtual void stop();
};


TestComponent::TestComponent()
    : orcaice::Component( "AppTest" )
{
}

TestComponent::~TestComponent()
{
}

void TestComponent::start()
{
    cout<<"testing Tracer::verbosity() ..."<<endl;
    {
        int verb;
        verb = context().tracer()->verbosity(hydroutil::Tracer::InfoTrace,hydroutil::Tracer::ToDisplay);
        if ( verb!=1 ) {
            cout<<"failed. wrong InfoToDisplay verbosity, expect=1, got="<<verb<<endl;
            exit(EXIT_FAILURE);
        }
        verb = context().tracer()->verbosity(hydroutil::Tracer::DebugTrace,hydroutil::Tracer::ToFile);
        if ( verb!=16 ) {
            cout<<"failed. wrong DebugToFile verbosity, expect=16, got="<<verb<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"excercising tracer ..."<<endl;
    context().tracer()->debug( "calling debug(1)", 1 );
    context().tracer()->info( "calling info()" );
    context().tracer()->warning( "calling warning()" );
    context().tracer()->error( "calling error()" );
    cout<<"ok";
    
    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    exit(EXIT_SUCCESS);
}

void TestComponent::stop()
{
    // nothing to do
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
