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
#include "powerI.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

PowerI::PowerI( const std::string& ifaceTag,
                const orcaice::Context& context ) :
    tag_(ifaceTag),
    context_(context)
{
}

PowerI::~PowerI()
{
}

void
PowerI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::PowerConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, tag_ );
}

::orca::PowerData 
PowerI::getData(const ::Ice::Current& ) const
{
    context_.tracer()->debug( "PowerI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PowerData data;
    dataProxy_.get( data );
    return data;
}

void 
PowerI::subscribe(const ::orca::PowerConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "PowerI::subscribe()", 5 );
    IceStorm::QoS qos;
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"PowerI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
PowerI::unsubscribe(const ::orca::PowerConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "PowerI::unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void
PowerI::localSet( const orca::PowerData& data )
{
    dataProxy_.set( data );
}

void
PowerI::localSetAndSend( const orca::PowerData& data )
{
//     cout<<"TRACE(PowerI.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::PowerConsumerPrx,orca::PowerData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
