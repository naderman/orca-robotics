/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "imuI.h"

using namespace std;
using namespace orca;

namespace imu {

ImuI::ImuI( const orca::ImuDescription& descr,
           const orcaice::Context & context )
    : descr_(descr),
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

orca::ImuData
ImuI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::ImuData data;
    // we don't need to pop the data here because we don't block on it.
    if ( imuDataStore_.isEmpty() )
    {
        cout << "ERROR(imuI.cpp): getData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Imu proxy is not populated yet" );
    }else{
        imuDataStore_.get( data );
    }

    return data;
}

orca::ImuDescription 
ImuI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getConfig()" << std::endl;
    return descr_;
}

orca::ImuDescription 
ImuI::localGetDescription() const
{
    return descr_;
}

// Subscribe people
void 
ImuI::subscribe(const ::orca::ImuConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
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
ImuI::localSetData( const ::orca::ImuData& data )
{
    // Stick it in the buffer so pullers can get it
    imuDataStore_.set( data );

    try {
        // push it to IceStorm
        imuPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException & )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer().warning( "Failed push to IceStorm." );
    }
}

} //namespace