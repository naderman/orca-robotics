/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "imuI.h"

using namespace std;
using namespace orca;
using namespace insgps;

ImuI::ImuI( ImuDescriptionPtr   descr,
            Driver*      hwDriver,
            orcaice::Context   context )
    : InsGpsI(context),
      descr_(descr),
      hwDriver_(hwDriver),
      imuData_(new ImuData),
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

void
ImuI::publish()
{
    // blocking read with timeout (2000ms by default)
    // get the imu position
    read( imuData_ );

    // send the data to icestorm and to a buffer for direct connections
    localSetData( imuData_ );

    return;
}

//
// local calls
//

void
ImuI::read( ::orca::ImuDataPtr& data )
{
    hwDriver_->readImu( data );
}         


//
// remote calls
//

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
::orca::ImuDescriptionPtr
ImuI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getConfig()" << std::endl;
    return descr_;
}

::orca::ImuDescriptionPtr
ImuI::localGetDescription() const
{
    return descr_;
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
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

