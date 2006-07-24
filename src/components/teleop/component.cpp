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

#include "component.h"
#include "networkhandler.h"
#include "userhandler.h"

using namespace std;
using namespace teleop;

Component::Component()
    : orcaice::Component( "Teleop", orcaice::HomeInterface ),
      networkHandler_(0),
      userHandler_(0)
{
}

Component::~Component()
{
    // do not delete networkHandler_ and userHandler_!!! They derive from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    tracer()->debug("starting component",5);

    // The only provided interfaces are the 2 standard ones: Home and Status.
    // We can just skip this activation step and they will not be visible on
    // on the network (if network traffic is an issue, for example).
    //
    // this may throw, but may as well quit right then
    activate();

    //
    // USER & DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
    userHandler_ = new UserHandler( &commandPipe_, context() );
    userHandler_->start();
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    networkHandler_ = new NetworkHandler( &commandPipe_, userHandler_->displayHandler(), context() );
    networkHandler_->start();
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
    orcaice::Thread::stopAndJoin( networkHandler_ );
    
    // userHandler_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    tracer()->info( "Component is quitting but the UserHandler is blocked waiting for user input.");
    tracer()->print( "************************************************" );
    tracer()->print( "Press any key or shake the joystick to continue." );
    tracer()->print( "************************************************" );
    
    orcaice::Thread::stopAndJoin( userHandler_ );
}
