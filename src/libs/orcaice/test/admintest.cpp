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
    TestComponent() : orcaice::Component( "Test", orcaice::AllStandardInterfaces ) {};

    // component interface
    virtual void start();
    virtual void stop() {};
};

void 
TestComponent::start()
{
    // we expect all admin interfaces to be activated

    cout<<"testing Tracer::verbosity() ..."<<endl;
    {
        int verb;
        verb = context().tracer().verbosity(gbxutilacfr::InfoTrace,gbxutilacfr::ToDisplay);
        if ( verb!=1 ) {
            cout<<"failed. wrong InfoToDisplay verbosity, expect=1, got="<<verb<<endl;
            exit(EXIT_FAILURE);
        }
        verb = context().tracer().verbosity(gbxutilacfr::DebugTrace,gbxutilacfr::ToFile);
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
//    activate();
//    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2000));

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
