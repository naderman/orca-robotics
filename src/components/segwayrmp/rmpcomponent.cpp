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

// include defnition of Ice runtime
#include <Ice/Ice.h>

#include "rmpcomponent.h"

// component FSM
#include "rmpFsm.h"

// implementations of Ice objects
#include "platform2d_i.h"
#include "power_i.h"

// segway rmp usb driver
#include "rmpusb/rmpusbdriver.h"

#include <orcaiceutil/ptrproxy.h>
#include <orcaiceutil/ptrbuffer.h>
#include <orcaiceutil/configutils.h>
#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

RmpComponent::RmpComponent() :
        driver_(0)
{
    fsm_ = new RmpFsm;
}

RmpComponent::~RmpComponent()
{
    delete driver_;
    delete fsm_;
}

void RmpComponent::start(const string & name,
             const Ice::CommunicatorPtr & communicator,
             const Ice::StringSeq & args)
{
    //
    // NETWORK-DRIVER INTERFACES
    //
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrProxy position2dProxy;
    // the driver will take the latest command from the proxy
    orcaiceutil::PtrProxy commandProxy;
    // the driver will put the latest data into this proxy
    orcaiceutil::PtrProxy powerProxy;
    // buffers for publishing service (for now, make it depth=1, essentially a proxy)
    orcaiceutil::PtrBuffer position2dBuffer( 1 );
    orcaiceutil::PtrBuffer powerBuffer( 1 );

    //
    // EXTERNAL INTERFACES
    //
    // create the one-and-only component adapter, parse config file to create adapter name
    if ( orcaiceutil::setComponentProperties( communicator, name ) ) {
        exit( EXIT_FAILURE );
    }
    adapter_ = communicator->createObjectAdapter(name);

    // PROVIDED INTERFACE: Platform2d
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr platform2dObj = new Platform2dI( &position2dProxy, &commandProxy );
    string iceObjName1 = orcaiceutil::getPortName( communicator, "Platform2d", name );
    adapter_->add( platform2dObj, Ice::stringToIdentity( iceObjName1 ) );

    // connect to IceStorm
    string topic1 = orcaiceutil::getTopicName( communicator, "Platform2d", name );
    Ice::ObjectPrx obj = orcaiceutil::getIceStormPublisher( communicator, topic1 );
    if ( !obj ) {
        exit( EXIT_FAILURE );
    }
    Position2dConsumerPrx position2dConsumer = Position2dConsumerPrx::uncheckedCast(obj);


    // PROVIDED INTERFACE: Power
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr powerObj = new PowerI( &powerProxy );
    string iceObjName2 = orcaiceutil::getPortName( communicator, "Power", name );
    adapter_->add( powerObj, Ice::stringToIdentity( iceObjName2 ) );

    // connect to IceStorm
    string topic2 = orcaiceutil::getTopicName( communicator, "Power", name );
    obj = orcaiceutil::getIceStormPublisher( communicator, topic2 );
    if ( !obj ) {
        exit( EXIT_FAILURE );
    }
    PowerConsumerPrx powerConsumer = PowerConsumerPrx::uncheckedCast(obj);

    //
    // HARDWARE INTERFACES
    //
    driver_ = new SegwayRmpUsb( &position2dBuffer, &powerBuffer,
                                 &position2dProxy, &commandProxy, &powerProxy );
    driver_->setup( communicator->getProperties() );
    driver_->activate();

    // we are ready, start processing external clients' requests
    adapter_->activate();

    // start pushing out data to IceStorm
    Ice::ObjectPtr data;
    orca::Position2dDataPtr posData = Position2dDataPtr::dynamicCast( data );
    while ( 1 )
    {
        // if the buffer is empty, this will block until next data arrives
        position2dBuffer.front( data );
        position2dBuffer.pop();

        posData = Position2dDataPtr::dynamicCast( data );
        //cout<<"push : "<<posData<<endl;

        // push it to IceStorm
        position2dConsumer->consumeData( posData );

        // power ...

        sleep(1);
    }

}

void RmpComponent::stop()
{
    cout<<"stopping component"<<endl;

    driver_->deactivate();

    adapter_->deactivate();
}
