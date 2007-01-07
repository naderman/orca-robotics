/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "cameraI.h"

using namespace std;
using namespace orca;

namespace imageserver {

CameraI::CameraI( const orca::CameraDescription& descr,
                  const std::string                &ifaceTag,
                  const orcaice::Context           & context )
    : descr_(descr),
      ifaceTag_(ifaceTag),
      context_(context)
{
//     currentConfig->format = ImageFormatModeRgb;
//     currentConfig->compression = ImageCompressionNone;

    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<CameraConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );
}

orca::CameraData
CameraI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "getData()", 5 );

    // we don't need to pop the data here because we don't block on it.
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    // create a null pointer. data will be cloned into it.
    orca::CameraData data;
    dataPipe_.get( data );

    return data;
}

// served out the data to the client (it was stored here earlier by the driver)
orca::CameraDescription
CameraI::getDescription(const Ice::Current& current) const
{
    context_.tracer()->debug( "getDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
CameraI::subscribe(const ::orca::CameraConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "subscribe()", 5 );

    if ( topicPrx_==0 ) {
        throw orca::SubscriptionFailedException( "null topic proxy." );
    }
    
    IceStorm::QoS qos;
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"CameraI::subscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( "failed to subscribe to an IceStorm topic." );
    }
}

// Unsubscribe people
void 
CameraI::unsubscribe(const ::orca::CameraConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void
CameraI::localSetData( const ::orca::CameraData& data )
{
    dataPipe_.push( data );

    // Try to push it out to IceStorm too
    try {
        consumerPrx_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        std::stringstream ss; ss << "Failed push to IceStorm: " << e;
        context_.tracer()->warning( ss.str() );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer()->info( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::CameraConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer()->print( "Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setData( data );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer()->print( "Re-connection to IceStorm failed." );
        }
    }
}

} // namespace
