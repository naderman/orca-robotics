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
#include "odometry3dI.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

Odometry3dI::Odometry3dI( const orca::VehicleDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    tag_(ifaceTag),
    context_(context)
{
}

Odometry3dI::~Odometry3dI()
{
}

void
Odometry3dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry3dConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, tag_ );
}

::orca::Odometry3dData 
Odometry3dI::getData(const ::Ice::Current& ) const
{
    context_.tracer()->debug( "Odometry3dI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Odometry3dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleDescription
Odometry3dI::getDescription(const ::Ice::Current& ) const
{
    return descr_;
}

void 
Odometry3dI::subscribe(const ::orca::Odometry3dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Odometry3dI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Odometry3dI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Odometry3dI::unsubscribe(const ::orca::Odometry3dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Odometry3dI::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Odometry3dI::localSet( const orca::Odometry3dData& data )
{
    dataProxy_.set( data );
}

void
Odometry3dI::localSetAndSend( const orca::Odometry3dData& data )
{
//     cout<<"TRACE(Odometry3dI.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Odometry3dConsumerPrx,orca::Odometry3dData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
