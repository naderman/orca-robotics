/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "ogmapI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace orcaifaceimpl {

OgMapI::OgMapI( const std::string      &ifaceTag,
                const orcaice::Context &context ) 
    : ifaceTag_(ifaceTag),
      context_(context)      
{
}

void
OgMapI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<OgMapConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

OgMapData
OgMapI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "OgMapI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    OgMapData data;
    dataProxy_.get( data );
    return data;
}

void
OgMapI::subscribe(const ::OgMapConsumerPrx& subscriber,
                  const Ice::Current& current)
{
    context_.tracer()->debug( "OgMapI::subscribe()", 5 );
    IceStorm::QoS qos;
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"OgMapI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
OgMapI::unsubscribe(const ::OgMapConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    context_.tracer()->debug( "OgMapI::unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void 
OgMapI::localSetAndSend( const ::orca::OgMapData &data )
{
    //cout<<"TRACE(ogmapI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<OgMapConsumerPrx,orca::OgMapData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

}
