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
#include "cameraI.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

CameraI::CameraI( const orca::CameraDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    tag_(ifaceTag),
    context_(context)
{
}

CameraI::~CameraI()
{
}

void
CameraI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::CameraConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, tag_ );
}

::orca::CameraData 
CameraI::getData(const ::Ice::Current& ) const
{
    context_.tracer()->debug( "CameraI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::CameraData data;
    dataProxy_.get( data );
    return data;
}

::orca::CameraDescription
CameraI::getDescription(const ::Ice::Current& ) const
{
    return descr_;
}

void 
CameraI::subscribe(const ::orca::CameraConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "CameraI::subscribe()", 5 );
    IceStorm::QoS qos;
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"CameraI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
CameraI::unsubscribe(const ::orca::CameraConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "CameraI::unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void
CameraI::localSet( const orca::CameraData& data )
{
    dataProxy_.set( data );
}

void
CameraI::localSetAndSend( const orca::CameraData& data )
{
//     cout<<"TRACE(CameraI.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::CameraConsumerPrx,orca::CameraData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
