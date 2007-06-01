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

// implementations of Ice objects
#include "powerI.h"
#include "odometry2dI.h"

#include "handler.h"

using namespace std;

namespace robot2dtester
{

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component() {};

    virtual void start();
    virtual void stop();

private:

    // main loop
    Handler* handler_;
};


Component::Component() : 
    orcaice::Component( "Robot2dTester" ),
    handler_(0)
{
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    //
    // PROVIDED : Position2dConsumer
    //
    // create a callback object to recieve scans
    Ice::ObjectPtr odometry2dConsumer = new Odometry2dConsumerI;
    orca::Odometry2dConsumerPrx odometry2dCallbackPrx =
        orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context(), odometry2dConsumer );

    //
    // Required : Power
    //
    // create a callback object to recieve scans
    Ice::ObjectPtr powerConsumer = new PowerConsumerI;
    orca::PowerConsumerPrx powerCallbackPrx =
        orcaice::createConsumerInterface<orca::PowerConsumerPrx>( context(), powerConsumer );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    handler_ = new Handler( odometry2dCallbackPrx, powerCallbackPrx, context() );
    handler_->start();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    tracer()->debug( "Stopping component", 2 );
    orcaice::stopAndJoin( handler_ );
    tracer()->debug( "Component stopped", 2 );
}

} // namespace

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    robot2dtester::Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
