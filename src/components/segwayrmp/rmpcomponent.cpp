/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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
#include "rmpmainloop.h"

// implementations of Ice objects
#include "platform2d_i.h"
#include "power_i.h"

#include <orcaiceutil/orcaiceutil.h>

using namespace orca;
using orcaiceutil::operator<<;

RmpComponent::RmpComponent() :
    orcaiceutil::Component( "SegwayRmp" ), mainLoop_(0)
{
}

RmpComponent::~RmpComponent()
{
    // do not delete mainLoop_!!! It derives from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void RmpComponent::start()
{
    //
    // EXTERNAL INTERFACES
    //

    // PROVIDED INTERFACE: Platform2d
    // create servant for direct connections and tell adapter about it
    platform2dObj_ = new Platform2dI( position2dBuffer_, commandBuffer_,
                                      setConfigBuffer_, currentConfigBuffer_ );
    std::string iceObjName1 = orcaiceutil::getPortName( current(), "Platform2d" );
    adapter()->add( platform2dObj_, Ice::stringToIdentity( iceObjName1 ) );

    // Find IceStorm ConsumerProxy to push out data
    orcaiceutil::getIceStormConsumerProxy<Position2dConsumerPrx>
                            ( current(), "Platform2d", position2dConsumer_ );


    // PROVIDED INTERFACE: Power
    // create servant for direct connections and tell adapter about it
    powerObj_ = new PowerI( powerBuffer_ );
    std::string iceObjName2 = orcaiceutil::getPortName( current(), "Power" );
    adapter()->add( powerObj_, Ice::stringToIdentity( iceObjName2 ) );

    // Find IceStorm ConsumerProxy to push out data
    orcaiceutil::getIceStormConsumerProxy<PowerConsumerPrx>
                            ( current(), "Power", powerConsumer_ );

    //
    // ENABLE ADAPTER
    //
    adapter()->activate();

    //
    // MAIN DRIVER LOOP
    //
    mainLoop_ = new RmpMainLoop( position2dBuffer_, commandBuffer_, powerBuffer_,
                                 setConfigBuffer_, currentConfigBuffer_,
                                 position2dConsumer_, powerConsumer_ );
    mainLoop_->setCurrent( current() );
    mainLoop_->start();

    // the rest is handled by the application/service
}

void RmpComponent::stop()
{
    logger()->trace("local", "stopping loop" );
    // Tell the main loop to stop
    mainLoop_->stop();

    logger()->trace("local", "joining thread" );
    // Then wait for it
    mainLoop_->getThreadControl().join();
}
