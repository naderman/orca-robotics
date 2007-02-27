#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"
#include "wifiI.h"

using namespace std;

namespace orcaifaceimpl {

WifiI::WifiI( const std::string       &ifaceTag, 
              const orcaice::Context  &context  )
    : ifaceTag_(ifaceTag),
      context_(context)
{
}

void
WifiI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::WifiConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}


::orca::WifiData
WifiI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "WifiI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::WifiData data;
    dataProxy_.get( data );
    return data;
}

void
WifiI::subscribe(const ::orca::WifiConsumerPrx& subscriber,
			const Ice::Current& current)
{   
    context_.tracer()->debug( "WifiI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"WifiI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
WifiI::unsubscribe(const ::orca::WifiConsumerPrx& subscriber,
			  const Ice::Current& current)
{
    context_.tracer()->debug( "WifiI::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
WifiI::localSetAndSend( const orca::WifiData& data )
{
    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::WifiConsumerPrx,orca::WifiData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

}

