#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"
#include "wifiImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class WifiI : virtual public ::orca::Wifi
{
public:

    WifiI( WifiImpl &impl )
        : impl_(impl) {}

    // remote functions

    virtual ::orca::WifiData getData(const Ice::Current&) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::WifiConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::WifiConsumerPrx& consumer,
                 const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

private:
    WifiImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

WifiImpl::WifiImpl( const std::string       &interfaceTag, 
                      const orcaice::Context  &context  )
    : interfaceTag_(interfaceTag),
      context_(context)
{
}

WifiImpl::~WifiImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
WifiImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::WifiConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new WifiI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}


::orca::WifiData
WifiImpl::internalGetData() const
{
    context_.tracer()->debug( "WifiImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::WifiData data;
    dataProxy_.get( data );
    return data;
}

void
WifiImpl::internalSubscribe(const ::orca::WifiConsumerPrx& subscriber)
{   
    context_.tracer()->debug( "WifiImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"WifiImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
WifiImpl::internalUnsubscribe(const ::orca::WifiConsumerPrx& subscriber)
{
    context_.tracer()->debug( "WifiImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
WifiImpl::localSetAndSend( const orca::WifiData& data )
{
    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::WifiConsumerPrx,orca::WifiData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceTag_ );
}

}
