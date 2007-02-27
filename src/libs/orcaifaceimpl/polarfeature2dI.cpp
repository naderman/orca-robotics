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
#include "polarfeature2dI.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

PolarFeature2dI::PolarFeature2dI( const std::string             &ifaceTag,
                                  const orcaice::Context        &context )
    : ifaceTag_(ifaceTag),
      context_(context)
{
}

void
PolarFeature2dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::PolarFeature2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

orca::PolarFeature2dDataPtr 
PolarFeature2dI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "PolarFeature2dI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::PolarFeature2dDataPtr data;
    dataProxy_.get( data );

    return data;
}

// Subscribe people
void 
PolarFeature2dI::subscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "PolarFeature2dI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

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
        ss <<"PolarFeature2dI::subscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
PolarFeature2dI::unsubscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "PolarFeature2dI::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
PolarFeature2dI::localSet( const ::orca::PolarFeature2dDataPtr &data )
{
    // cout << "PolarFeature2dI::set data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );
}

void
PolarFeature2dI::localSetAndSend( const ::orca::PolarFeature2dDataPtr &data )
{
    if ( context_.tracer()->verbosity( orcaice::Tracer::DebugTrace, orcaice::Tracer::ToAny ) >= 5 )
    {
        stringstream ss;
        ss << "PolarFeature2dI: Sending data: " << orcaice::toString(data);
        context_.tracer()->debug( ss.str(), 5 );
    }

    // cout << "PolarFeature2dI::set data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );

    // Try to push to IceStorm
    tryPushToIceStormWithReconnect<PolarFeature2dConsumerPrx,PolarFeature2dDataPtr>( context_,
                                                                                     consumerPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     ifaceTag_ );
}

} // namespace
