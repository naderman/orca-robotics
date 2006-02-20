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

#include "rmpcomponent.h"
#include "nethandler.h"
#include "hwhandler.h"

#include <orcaice/orcaice.h>

using namespace orca;

RmpComponent::RmpComponent() :
    orcaice::Component( "SegwayRmp" ), netHandler_(0), hwHandler_(0)
{
}

RmpComponent::~RmpComponent()
{
    // do not delete handlers!!! They derive from Ice::Thread and delete itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void RmpComponent::start()
{
    //
    // Network handling loop
    //
    netHandler_ = new NetHandler( position2dBuffer_, commandBuffer_, powerBuffer_,
                                  setConfigBuffer_, currentConfigBuffer_, context() );
    netHandler_->start();

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // Hardware handling loop
    //
    hwHandler_ = new HwHandler( position2dBuffer_, commandBuffer_, powerBuffer_,
                                setConfigBuffer_, currentConfigBuffer_, context() );
    hwHandler_->start();

    // the rest is handled by the application/service
}

void RmpComponent::stop()
{
    // it's possible that either or both of the handlers are not even created
    // if exceptions are raised in ttheir constructor
    if ( netHandler_ ) {
        IceUtil::ThreadControl netControl = netHandler_->getThreadControl();
        tracer()->debug("stopping net handler", 5 );
        netHandler_->stop();
        tracer()->debug("joining net handler", 5 );
        netControl.join();
    }
    if ( hwHandler_ ) {
        IceUtil::ThreadControl hwControl = hwHandler_->getThreadControl();
        tracer()->debug("stopping hw handler", 5 );
        hwHandler_->stop();
        tracer()->debug("joining hw handler", 5 );
        hwControl.join();
    }
}
