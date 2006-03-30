/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
void Component::start()
{
    context().tracer()->debug("starting component",5);

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

void Component::stop()
{
    orcaice::Thread::stopAndJoin( networkHandler_ );
    
    // userHandler_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    cout<<"Quitting... Press any key or shake the joystick to continue."<<endl;
    
    orcaice::Thread::stopAndJoin( userHandler_ );
}
