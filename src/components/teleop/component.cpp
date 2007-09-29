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

#include "component.h"
#include "networkhandler.h"
#include "termdisplayhandler.h"
#include "inputhandler.h"

using namespace std;
using namespace teleop;

Component::Component() :
    orcaice::Component( "Teleop", orcaice::HomeInterface ),
    networkHandler_(0),
    displayHandler_(0),
    inputHandler_(0)
{
}

Component::~Component()
{
    // do not delete networkHandler_ and displayHandler_!!! They derive from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    tracer()->debug("starting component",2);

    //
    // USER DISPLAY
    //
    // the constructor may throw, we'll let the application shut us down
    displayHandler_ = new TermDisplayHandler( context() );
    displayHandler_->start();
    
    //
    // NETWORK
    //
    // the constructor may throw, we'll let the application shut us down
    networkHandler_ = new NetworkHandler( (Display*)displayHandler_, context() );
    networkHandler_->start();

    //
    // USER INPUT
    //
    // the constructor may throw, we'll let the application shut us down
    inputHandler_ = new InputHandler( (Network*)networkHandler_, context() );
    inputHandler_->start();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    // inputHandler_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    tracer()->info( "Component is quitting but the InputHandler is blocked waiting for user input.");
    tracer()->print( "************************************************" );
    tracer()->print( "Press any key or shake the joystick to continue." );
    tracer()->print( "************************************************" );
    
    orcaiceutil::stopAndJoin( inputHandler_ );
    orcaiceutil::stopAndJoin( networkHandler_ );
    orcaiceutil::stopAndJoin( displayHandler_ );
}
