/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006 Ben Upcroft
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
#include <orcaice/orcaice.h>

#include "position3dI.h"

using namespace std;
using namespace orca;

Position3dI::Position3dI( Position3dGeometryPtr   geometry,
                          orcaice::Context        context )
    :   geometry_(geometry),
        context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Imu
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<Position3dConsumerPrx>
            ( context_, position3dPublisher_, "Position3d" );
}

orca::Position3dDataPtr
Position3dI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::Position3dDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( position3dDataBuffer_.isEmpty() )
    {
        cout << "ERROR(position3dI.cpp): getData() was called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Position3d proxy is not populated yet" );
    }else{
        position3dDataBuffer_.get( data );
    }

    return data;
}


// Get Imu Geometry
::orca::Position3dGeometryPtr
Position3dI::getGeometry(const ::Ice::Current& ) const
{
    std::cout << "getGeometry()" << std::endl;
    return geometry_;
}

// Subscribe people
void 
Position3dI::subscribe(const ::orca::Position3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
Position3dI::unsubscribe(const ::orca::Position3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe() Position3d" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set pva IMU Data
void
Position3dI::localSetData( ::orca::Position3dDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    position3dDataBuffer_.push( data );

    try {
        // push it to IceStorm
        position3dPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

