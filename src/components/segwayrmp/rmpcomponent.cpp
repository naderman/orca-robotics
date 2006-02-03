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

#include <IceStorm/IceStorm.h>

#include "rmpcomponent.h"
#include "rmpmainloop.h"

// implementations of Ice objects
#include "platform2d_i.h"
#include "power_i.h"

#include <orcaiceutil/orcaiceutil.h>

using namespace orca;

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
    IceStorm::TopicPrx platfTopicPrx = orcaiceutil::connectToIceStormTopicWithTag<Position2dConsumerPrx>
                ( current(), position2dPublisher_, "Platform2d" );

    // create servant for direct connections and tell adapter about it
    platform2dObj_ = new Platform2dI( position2dBuffer_, commandBuffer_,
                                      setConfigBuffer_, currentConfigBuffer_, platfTopicPrx );
    orcaiceutil::createInterfaceWithTag( current(), platform2dObj_, "Platform2d" );



    // PROVIDED INTERFACE: Power
    // Find IceStorm ConsumerProxy to push out data
    IceStorm::TopicPrx powerTopicPrx = orcaiceutil::connectToIceStormTopicWithTag<PowerConsumerPrx>
                ( current(), powerPublisher_, "Power" );
    
    // create servant for direct connections and tell adapter about it
    powerObj_ = new PowerI( powerBuffer_, powerTopicPrx );
    orcaiceutil::createInterfaceWithTag( current(), powerObj_, "Power" );    


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

    tracer()->debug("stopping loop", 5 );
    // Tell the main loop to stop
    mainLoop_->stop();

    tracer()->debug("joining thread", 5 );
    // Then wait for it
    mainControl.join();
}
