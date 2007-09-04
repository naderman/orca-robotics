/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "odometry3dI.h"

using namespace std;
using namespace orca;
using namespace insgps;

Odometry3dI::Odometry3dI( const VehicleDescription&  descr,
                          Driver*                   hwDriver,
                          const orcaice::Context & context )
    :   InsGpsI(context),
        odometry3dDataBuffer_(100,orcaice::BufferTypeCircular),
        descr_(descr),
        hwDriver_(hwDriver),
        context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Odometry3d
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<Odometry3dConsumerPrx>
            ( context_, odometry3dPublisher_, "Odometry3d" );
}

void
Odometry3dI::publish()
{
   
    // blocking read with timeout (2000ms by default)
    // get odometry3d
    read( odometry3dData_ );
    
    // send the data to icestorm and to a buffer for direct connections
    localSetData( odometry3dData_ );
            
    return;
}

//
// local calls
//

void
Odometry3dI::read( ::orca::Odometry3dData& data )
{
    hwDriver_->readOdometry3d( data );
}         


//
// remote calls
//

orca::Odometry3dData
Odometry3dI::getData(const Ice::Current& current) const
{
//     std::cout << "getData()" << std::endl;
    orca::Odometry3dData data;
    // we don't need to pop the data here because we don't block on it.
    if ( odometry3dDataBuffer_.isEmpty() )
    {
        cout << "ERROR(odometry3dI.cpp): getData() was called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Odometry3d proxy is not populated yet" );
    }else{
        odometry3dDataBuffer_.get( data );
    }
    return data;
}


::orca::VehicleDescription
Odometry3dI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getDescription()" << std::endl;
    return descr_;
}

// Subscribe people
void 
Odometry3dI::subscribe(const ::orca::Odometry3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
Odometry3dI::unsubscribe(const ::orca::Odometry3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe() Odometry3d" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set pva IMU Data
void
Odometry3dI::localSetData( const ::orca::Odometry3dData& data )
{
    // Stick it in the buffer so pullers can get it
    odometry3dDataBuffer_.push( data );

    try {
        // push it to IceStorm
        odometry3dPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

