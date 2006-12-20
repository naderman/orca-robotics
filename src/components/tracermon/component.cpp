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
using namespace tracermon;

Component::Component()
    : orcaice::Component( "TracerMon", orcaice::HomeInterface ),
      netHandler_(0),
      usrHandler_(0)
{
}

Component::~Component()
{
    // do not delete networkHandler_ and userHandler_!!! 
    // They derive from Ice::Thread and delete itself.
}

void
Component::start()
{
    // this may throw, but may as well quit right then
    activate();

    //
    // EVENT QUEUES
    //
    netQueue_ = new EventQueue();    
    usrQueue_ = new EventQueue();    

    //
    // USER & DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
    usrHandler_ = new UserHandler( usrQueue_, netQueue_, context() );
    usrHandler_->start();
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    netHandler_ = new NetworkHandler( netQueue_, usrQueue_, context() );
    netHandler_->start();
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
    orcaice::Thread::stopAndJoin( netHandler_ );

    // userHandler_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    tracer()->info( "Component is quitting but the UserHandler is blocked waiting for user input.");
    tracer()->print( "************************************************" );
    tracer()->print( "Press any key or shake the joystick to continue." );
    tracer()->print( "************************************************" );
    
    orcaice::Thread::stopAndJoin( usrHandler_ );
}
