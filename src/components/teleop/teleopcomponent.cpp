/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include "teleopcomponent.h"
#include "networkloop.h"
#include "inputloop.h"    

using namespace std;

TeleopComponent::TeleopComponent()
    : orcaiceutil::Component( "Teleop" )
{
}

TeleopComponent::~TeleopComponent()
{
    // do not delete networkLoop_ and inputLoop_!!! They derive from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void TeleopComponent::start()
{
    //
    // NETWORK
    //
    networkLoop_ = new NetworkLoop( &commandProxy_ );
    networkLoop_->setupComms( communicator(), componentName() );
    networkLoop_->setupConfigs( communicator()->getProperties() );
    networkLoop_->start();

    //
    // HARDWARE
    //
    inputLoop_ = new InputLoop( &commandProxy_ );
    inputLoop_->setupConfigs( communicator()->getProperties() );
    inputLoop_->start();
    
    // the rest is handled by the application/service
}

void TeleopComponent::stop()
{
    networkLoop_->stop();
    inputLoop_->stop();

    networkLoop_->getThreadControl().join();

    // inputLoop_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    cout<<"Quitting... Press any key to continue."<<endl;

    inputLoop_->getThreadControl().join();
}
