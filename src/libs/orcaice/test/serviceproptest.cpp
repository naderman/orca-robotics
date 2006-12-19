/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/service.h>
#include <orcaice/component.h>

#include <orcaice/orcaice.h>

// is this dodgy?
#include "../detail/homeI.h"

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "ServiceTest" ) {};
    virtual ~TestComponent() {};

    // component interface
    virtual void start();
    virtual void stop() {};
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
    int got = properties()->getPropertyAsInt( "Orca.Tracer.ErrorToFile" );
    int expect = properties()->getPropertyAsInt( tag()+".Expect" );
    if ( got != expect ) {
        cout<<"failed"<<endl<<"expect="<<expect<<"; got="<<got<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    if ( properties()->getPropertyAsInt( tag()+".Config.Quit" ) )
    {
        cout<<"quitting..."<<endl;
        // NOTE: cannot call communicator()->destroy() from here
        // because they'll be caught by Ice::Application and show up as failed ctest.
        exit(EXIT_SUCCESS);
    }
}
