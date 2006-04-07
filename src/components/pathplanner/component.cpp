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

#include "component.h"
#include "algohandler.h"

using namespace pathplanner;

Component::Component()
    : orcaice::Component( "PathPlanner" ),
      algoHandler_(0)
{
}

Component::~Component()
{
    // do not delete handlers!!! They derives from Ice::Thread and delete themselves.
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void Component::start()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // only needed for Home and Status interfaces
    // this may throw, but may as well quit right then
    activate();

    //
    // Hardware handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    algoHandler_ = new AlgoHandler( context() );
    algoHandler_->start();
    
    // the rest is handled by the application/service
}

void Component::stop()
{
    if ( algoHandler_ ) {
        IceUtil::ThreadControl algoControl = algoHandler_->getThreadControl();
        tracer()->debug("stopping algorithm handler", 5 );
        algoHandler_->stop();
        tracer()->debug("joining algorithm handler", 5 );
        algoControl.join();
    }
}
