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

// Ice object definitions
//#include <orca/home.h>
//#include <orca/status.h>

// Ice objects implementations
#include "statusI.h"

#include "testloop.h"

using namespace std;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    TestLoop* testLoop_;
};


Component::Component()
    : orcaice::Component( "OrcaIceTester" ),
      testLoop_(0)
{
}

Component::~Component()
{
    // do not delete testLoop_!!! It derives from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void Component::start()
{
    //
    // provided : StatusConsumer
    //
    // create servant and tell adapter about it (let it make up a globally unique name)
    Ice::ObjectPrx obj = context().adapter()->addWithUUID( new StatusConsumerI );
    // make a direct proxy
    Ice::ObjectPrx prx = context().adapter()->createDirectProxy( obj->ice_getIdentity() );
    orca::StatusConsumerPrx statusCallbackPrx = orca::StatusConsumerPrx::uncheckedCast( prx );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    testLoop_ = new TestLoop( statusCallbackPrx, context() );
    testLoop_->start();

    // the rest is handled by the application/service
}

void Component::stop()
{
    // get a handle on the loop thread before it's stopped. This way we can join it even if it's already gone.
    IceUtil::ThreadControl testControl = testLoop_->getThreadControl();

    tracer()->print( "stopping loop" );
    // Tell the main loop to stop
    testLoop_->stop();

    tracer()->print( "joining thread" );
    // Then wait for it
    testControl.join();
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
