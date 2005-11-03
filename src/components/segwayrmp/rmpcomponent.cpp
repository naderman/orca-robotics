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

#include <IceStorm/IceStorm.h>

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
    // Find IceStorm Topic to which we'll publish
    IceStorm::TopicPrx topicPrx = orcaiceutil::connectToIceStormConsumer<Position2dConsumerPrx>
                ( current(), position2dPublisher_, "Platform2d" );

    // create servant for direct connections and tell adapter about it
    platform2dObj_ = new Platform2dI( position2dBuffer_, commandBuffer_,
                                      setConfigBuffer_, currentConfigBuffer_, topicPrx );
    orcaiceutil::createInterface( current(), platform2dObj_, "Platform2d" );



    // PROVIDED INTERFACE: Power
    // create servant for direct connections and tell adapter about it
    powerObj_ = new PowerI( powerBuffer_ );
    adapter()->add( powerObj_, orcaiceutil::getProvidedNameAsIdentity(current(),"Power") );

    // Find IceStorm ConsumerProxy to push out data
    orcaiceutil::connectToIceStormConsumer<PowerConsumerPrx> ( current(), powerPublisher_, "Power" );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    mainLoop_ = new RmpMainLoop( position2dBuffer_, commandBuffer_, powerBuffer_,
                                 setConfigBuffer_, currentConfigBuffer_,
                                 position2dPublisher_, powerPublisher_ );
    mainLoop_->setCurrent( current() );
    mainLoop_->start();

    // the rest is handled by the application/service
}

void RmpComponent::stop()
{
    IceUtil::ThreadControl mainControl = mainLoop_->getThreadControl();

    logger()->trace("local", "stopping loop" );
    // Tell the main loop to stop
    mainLoop_->stop();

    logger()->trace("local", "joining thread" );
    // Then wait for it
    mainControl.join();
}
