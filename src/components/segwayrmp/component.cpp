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

//#include <iostream> // debug
#include <orcaice/orcaice.h>

#include "component.h"
#include "nethandler.h"
#include "hwhandler.h"

using namespace orca;
using namespace segwayrmp;

Component::Component() :
    orcaice::Component( "SegwayRmp" ),
    netHandler_(0),
    hwHandler_(0)
{
}

Component::~Component()
{
    // do not delete handlers!!! They derive from Ice::Thread and self-destruct.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void
Component::start()
{
    //
    // Network handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    netHandler_ = new NetHandler( position2dPipe_, commandPipe_, powerPipe_,
                                setConfigPipe_, currentConfigPipe_, context() );
    netHandler_->start();

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw, but may as well quit right then
    activate();

    //
    // Hardware handling loop
    //
    // the constructor may throw, we'll let the application shut us down
    hwHandler_ = new HwHandler( position2dPipe_, commandPipe_, powerPipe_,
                                setConfigPipe_, currentConfigPipe_, context() );
    hwHandler_->start();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    tracer()->debug( "stopping component", 5 );
    orcaice::Thread::stopAndJoin( netHandler_ );
    orcaice::Thread::stopAndJoin( hwHandler_ );
    tracer()->debug( "stopped component", 5 );
}
