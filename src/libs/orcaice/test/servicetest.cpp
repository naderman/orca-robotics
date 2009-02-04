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
#include <orcaice/service.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "ServiceTest", orcaice::NoStandardInterfaces ) {};
    virtual void start();
    virtual void stop();
};

void TestComponent::start()
{
    cout<<"testing Service() ... ok"<<endl;
    
    // no use! the IceBox will not quit anyway.
//     bool forceServiceShutdown = true;
//     context().shutdown( forceServiceShutdown );
// 
//     cout<<"communicator isShutdown="<<(int)context().communicator()->isShutdown()<<endl;

    exit(EXIT_SUCCESS);
}

void TestComponent::stop()
{
    // nothing to do
}

class TestService : public orcaice::Service
{
public:
    TestService()
    {
        component_ = new TestComponent;
    };
};

extern "C"
{
    //
    // Factory function
    //
    IceBox::Service* create( Ice::CommunicatorPtr )
    {
        return new TestService;
    }
}
