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
#include "odometry2dI.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

Odometry2dI::Odometry2dI( const orca::VehicleDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    tag_(ifaceTag),
    context_(context)
{
}

Odometry2dI::~Odometry2dI()
{
}

void
Odometry2dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry2dConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, tag_ );
}

::orca::Odometry2dData 
Odometry2dI::getData(const ::Ice::Current& ) const
{
    context_.tracer()->debug( "Odometry2dI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Odometry2dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleDescription
Odometry2dI::getDescription(const ::Ice::Current& ) const
{
    return descr_;
}

void 
Odometry2dI::subscribe(const ::orca::Odometry2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Odometry2dI::subscribe()", 5 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Odometry2dI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Odometry2dI::unsubscribe(const ::orca::Odometry2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Odometry2dI::unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void
Odometry2dI::localSet( const orca::Odometry2dData& data )
{
    dataProxy_.set( data );
}

void
Odometry2dI::localSetAndSend( const orca::Odometry2dData& data )
{
//     cout<<"TRACE(Odometry2dI.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Odometry2dConsumerPrx,orca::Odometry2dData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
