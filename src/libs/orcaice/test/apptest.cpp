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
    cout<<"testing compoenent interface settings ... ";
    {
        // ON implicitly
        bool enabled;
        enabled = interfaceFlag() & orcaice::TracerInterface;
        if ( enabled != true ) {
            cout<<"failed"<<endl<<"expected Tracer to be enabled, flag="<<interfaceFlag()<<endl;
            exit(EXIT_FAILURE);
        }
        // ON explicitly
        enabled = interfaceFlag() & orcaice::StatusInterface;
        if ( enabled != true ) {
            cout<<"failed"<<endl<<"expected Status to be enabled, flag="<<interfaceFlag()<<endl;
            exit(EXIT_FAILURE);
        }
        // OFF explicitly
        enabled = interfaceFlag() & orcaice::HomeInterface;
        if ( enabled != false ) {
            cout<<"failed"<<endl<<"expected Home to be disabled, flag="<<interfaceFlag()<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing Tracer::verbosity() ..."<<endl;
    {
        int verb;
        verb = context().tracer().verbosity(hydroutil::Tracer::InfoTrace,hydroutil::Tracer::ToDisplay);
        if ( verb!=1 ) {
            cout<<"failed. wrong InfoToDisplay verbosity, expect=1, got="<<verb<<endl;
            exit(EXIT_FAILURE);
        }
        verb = context().tracer().verbosity(hydroutil::Tracer::DebugTrace,hydroutil::Tracer::ToFile);
        if ( verb!=16 ) {
            cout<<"failed. wrong DebugToFile verbosity, expect=16, got="<<verb<<endl;
            exit(EXIT_FAILURE);
        }
    }
    cout<<"ok"<<endl;

    cout<<"excercising tracer ..."<<endl;
    context().tracer().debug( "calling debug(1)", 1 );
    context().tracer().info( "calling info()" );
    context().tracer().warning( "calling warning()" );
    context().tracer().error( "calling error()" );
    cout<<"ok"<<endl;
    
    cout<<"excercising status ..."<<endl;
    // wait a bit for the ComponentThread to call process()
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));

    context().status().addSubsystem( "core" );
    context().status().setMaxHeartbeatInterval( "core", 10 );
    context().status().initialising( "core", "holding fingers crossed" );
    context().status().heartbeat( "core" );
    context().status().ok( "core", "all ok" );
    context().status().warning( "core", "all is weird" );
    context().status().fault( "core", "all is bad" );
    // ComponentThread usually does it once per second
    context().status().process();
    cout<<"ok"<<endl;

    // uncomment to manually probe Status interface
//     activate();
//     IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2000));

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
