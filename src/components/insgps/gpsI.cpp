/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "gpsI.h"

using namespace std;
using namespace orca;
using namespace insgps;

GpsI::GpsI( const orca::GpsDescription& descr,
            Driver*      hwDriver,
            const orcaice::Context & context )
    : InsGpsI(context),
      // alexm: using the same settings as in FakeInsGpsDriver 
      gpsDataBuffer_(100,hydroutil::BufferTypeQueue),
      descr_(descr),
      hwDriver_(hwDriver),
      context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Gps
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<GpsConsumerPrx>
        ( context_, gpsPublisher_, "Gps" );
}

void
GpsI::publish()
{
    // blocking read with timeout (2000ms by default)
    // get the gps position
    read( gpsData_ );
            
    // send the data to icestorm and to a buffer for direct connections
    localSetData( gpsData_ );

    return;
}

//
// local calls
//

void
GpsI::read( ::orca::GpsData& data )
{
    // cout << "TRACE(gpsi::read()): in read()" << endl;
    hwDriver_->readGps( data );
    // cout << "TRACE(gpsi::read()): after read()" << endl;
}         


//
// remote calls
//

orca::GpsData
GpsI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsData data;
    // we don't need to pop the data here because we don't block on it.
    if ( gpsDataBuffer_.isEmpty() )
    {
        cout << "ERROR(gpsI.cpp): getData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Gps proxy is not populated yet" );
    }else{
        gpsDataBuffer_.get( data );
    }

    return data;
}

::orca::GpsDescription
GpsI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getDescription()" << std::endl;
    return descr_;
}

::orca::GpsDescription
GpsI::localGetDescription() const
{
    return descr_;
}

// Subscribe people
void 
GpsI::subscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
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
GpsI::unsubscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe()" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set GPS Data
void
GpsI::localSetData( const ::orca::GpsData& data )
{
    // Stick it in the buffer so pullers can get it
    gpsDataBuffer_.push( data );

    try {
        // push it to IceStorm
        gpsPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

