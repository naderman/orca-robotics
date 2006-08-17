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

#include "imuI.h"

using namespace std;
using namespace orca;

ImuI::ImuI(ImuConfigDataPtr   config,
           orcaice::Context   context )
    : config_(config),
    context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Imu
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<ImuConsumerPrx>
        ( context_, imuPublisher_, "Imu" );
}

orca::ImuDataPtr
ImuI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::ImuDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( imuDataBuffer_.isEmpty() )
    {
        cout << "ERROR(imuI.cpp): getData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Imu proxy is not populated yet" );
    }else{
        imuDataBuffer_.get( data );
    }

    return data;
}

// Get Imu Geometry
::orca::ImuConfigDataPtr
ImuI::getConfig(const ::Ice::Current& ) const
{
    std::cout << "getConfig()" << std::endl;
    return config_;
}

::orca::ImuConfigDataPtr
ImuI::localGetConfig() const
{
    return config_;
}

// Subscribe people
void 
ImuI::subscribe(const ::orca::ImuConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
ImuI::unsubscribe(const ::orca::ImuConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe()" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set raw IMU Data
void
ImuI::localSetData( ::orca::ImuDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    imuDataBuffer_.push( data );

    try {
        // push it to IceStorm
        imuPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException & )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

