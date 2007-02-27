/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "localise3dI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

Localise3dI::Localise3dI( const std::string &ifaceTag,
                          const orcaice::Context &context ) :
    ifaceTag_(ifaceTag),
    context_(context)
{
}

void
Localise3dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Localise3dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

::orca::Localise3dData 
Localise3dI::getData(const ::Ice::Current& ) const
{
    context_.tracer()->debug( "Localise3dI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Localise3dData data;
    dataProxy_.get( data );
    return data;
}

void 
Localise3dI::subscribe(const ::orca::Localise3dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Localise3dI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Localise3dI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise3dI::unsubscribe(const ::orca::Localise3dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Localise3dI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise3dI::localSet( const orca::Localise3dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
}

void
Localise3dI::localSetAndSend( const orca::Localise3dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Localise3dConsumerPrx,orca::Localise3dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

} // namespace
