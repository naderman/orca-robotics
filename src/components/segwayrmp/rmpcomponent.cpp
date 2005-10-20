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

#include <iostream>

#include "rmpcomponent.h"
#include "rmpmainloop.h"

// implementations of Ice objects
#include "platform2d_i.h"
#include "power_i.h"

#include <orcaiceutil/orcaiceutil.h>

using namespace std;
using namespace orca;
using orcaiceutil::operator<<;

RmpComponent::RmpComponent() :
    orcaiceutil::Component( "SegwayRmp" ), mainLoop_(0)
{
}

RmpComponent::~RmpComponent()
{
    delete mainLoop_;
}

int RmpComponent::go()
{
    //
    // NETWORK-MAINLOOP INTERFACES
    //
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer position2dProxy;
    // the driver will take the latest command from the proxy
    orcaiceutil::PtrBuffer commandProxy;
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer powerProxy;

    //
    // EXTERNAL INTERFACES
    //

    // PROVIDED INTERFACE: Platform2d
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr platform2dObj = new Platform2dI( &position2dProxy, &commandProxy );
    string iceObjName1 = orcaiceutil::getPortName( communicator(), "Platform2d", adapterName() );
    adapter()->add( platform2dObj, Ice::stringToIdentity( iceObjName1 ) );

    // Find IceStorm ConsumerProxy to push out data
    Position2dConsumerPrx position2dConsumer;
    orcaiceutil::getIceStormConsumerProxy<Position2dConsumerPrx>
            ( communicator(), adapterName(), "Platform2d", position2dConsumer );


    // PROVIDED INTERFACE: Power
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr powerObj = new PowerI( &powerProxy );
    string iceObjName2 = orcaiceutil::getPortName( communicator(), "Power", adapterName() );
    adapter()->add( powerObj, Ice::stringToIdentity( iceObjName2 ) );

    // Find IceStorm ConsumerProxy to push out data
    PowerConsumerPrx powerConsumer;
    orcaiceutil::getIceStormConsumerProxy<PowerConsumerPrx>
            ( communicator(), adapterName(), "Power", powerConsumer );

    //
    // ENABLE ADAPTER
    //
    adapter()->activate();

    //
    // MAIN DRIVER LOOP
    //
    mainLoop_ = new RmpMainLoop( &position2dProxy, &commandProxy, &powerProxy,
                                 position2dConsumer, powerConsumer );

    mainLoop_->setupConfigs( communicator()->getProperties() );
    mainLoop_->start();

    communicator()->waitForShutdown();

    stop();

    cout<<"fully stopped"<<endl;

    return 0;
}

void RmpComponent::stop()
{
    cout<<"stopping component"<<endl;
    //adapter()->deactivate();

    mainLoop_->stop();
    cout<<"joining main loop ..."<<endl;
    mainLoop_->getThreadControl().join();
}
