/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "pixmapI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

PixMapI::PixMapI( const std::string      &ifaceTag,
                const orcaice::Context &context ) 
    : ifaceTag_(ifaceTag),
      context_(context)      
{
}

void
PixMapI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<PixMapConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

PixMapData
PixMapI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "PixMapI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    PixMapData data;
    dataProxy_.get( data );
    return data;
}

void
PixMapI::subscribe(const ::PixMapConsumerPrx& subscriber,
                  const Ice::Current& current)
{
    context_.tracer()->debug( "PixMapI::subscribe()", 5 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"PixMapI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
PixMapI::unsubscribe(const ::PixMapConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    context_.tracer()->debug( "PixMapI::unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void 
PixMapI::localSetAndSend( const ::orca::PixMapData &data )
{
    //cout<<"TRACE(ogmapI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<PixMapConsumerPrx,orca::PixMapData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

}
