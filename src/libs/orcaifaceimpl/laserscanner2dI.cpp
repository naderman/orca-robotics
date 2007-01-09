/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaifaceimpl/util.h>
#include "laserscanner2dI.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

LaserScanner2dI::LaserScanner2dI( const orca::RangeScanner2dDescription& descr,
                                  const std::string             &ifaceTag,
                                  const orcaice::Context        &context )
    : descr_(descr),
      ifaceTag_(ifaceTag),
      context_(context)
{
}

void
LaserScanner2dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

orca::RangeScanner2dDataPtr 
LaserScanner2dI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "LaserScanner2dI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::LaserScanner2dDataPtr data;
    dataProxy_.get( data );

    return data;
}

// served out the data to the client (it was stored here earlier by the driver)
orca::RangeScanner2dDescription
LaserScanner2dI::getDescription(const Ice::Current& current) const
{
    context_.tracer()->debug( "LaserScanner2dI::getDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
LaserScanner2dI::subscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "LaserScanner2dI::subscribe()", 5 );

    if ( topicPrx_==0 ) {
        throw orca::SubscriptionFailedException( "null topic proxy." );
    }
    
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"LaserScanner2dI::subscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
LaserScanner2dI::unsubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "LaserScanner2dI::unsubscribe()", 5 );

    topicPrx_->unsubscribe( subscriber );
}

void
LaserScanner2dI::localSet( const ::orca::LaserScanner2dDataPtr &data )
{
    // cout << "LaserScanner2dI::set data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );
}

void
LaserScanner2dI::localSetAndSend( const ::orca::LaserScanner2dDataPtr &data )
{
    // cout << "LaserScanner2dI::set data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );

    // Try to push to IceStorm
    tryPushToIceStormWithReconnect<RangeScanner2dConsumerPrx,LaserScanner2dDataPtr>( context_,
                                                                                     consumerPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     ifaceTag_ );
}

} // namespace
