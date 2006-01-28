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

#include "teleopcomponent.h"
#include "networkhandler.h"
#include "userhandler.h"    

using namespace std;

TeleopComponent::TeleopComponent()
    : orcaiceutil::Component( "Teleop" ), networkHandler_(0), userHandler_(0)
{
}

TeleopComponent::~TeleopComponent()
{
    // do not delete networkHandler_ and userHandler_!!! They derive from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void TeleopComponent::start()
{
    current().tracer()->debug("starting component");
    //
    // NETWORK
    //
    networkHandler_ = new NetworkHandler( &commandProxy_ );
    networkHandler_->setCurrent( current() );
    networkHandler_->start();

    //
    // HARDWARE
    //
    userHandler_ = new UserHandler( &commandProxy_ );
    userHandler_->setCurrent( current() );
    userHandler_->start();
    
    // the rest is handled by the application/service
}

void TeleopComponent::stop()
{
    IceUtil::ThreadControl networkControl = networkHandler_->getThreadControl();
    IceUtil::ThreadControl inputControl = userHandler_->getThreadControl();

    networkHandler_->stop();
    userHandler_->stop();

    networkControl.join();

    // userHandler_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    cout<<"Quitting... Press any key or shake the joystick to continue."<<endl;

    inputControl.join();
}
