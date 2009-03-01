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
#include <orcaice/service.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "ServiceTest", orcaice::NoStandardInterfaces ) {};
    virtual void start();
};

class TestService : public orcaice::Service
{
public:
    TestService() { component_ = new TestComponent; };
};

extern "C"
{
    //
    // Factory function
    //
    IceBox::Service* create( Ice::CommunicatorPtr communicator )
    {
        return new TestService;
    }
}

void TestComponent::start()
{
    cout<<"testing properties in Service ... ";

    int got = context().properties()->getPropertyAsInt( "Orca.Tracer.ErrorToFile" );

    string prefix = context().tag()+".Config.";
    int expect = context().properties()->getPropertyAsInt( prefix+"Expect" );

    if ( got != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<got<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    if ( context().properties()->getPropertyAsInt( prefix+"Quit" ) )
    {
        cout<<"quitting..."<<endl;
//         bool forceServiceShutdown = true;
//         context().shutdown( forceServiceShutdown );
        exit(EXIT_SUCCESS);
    }
}
