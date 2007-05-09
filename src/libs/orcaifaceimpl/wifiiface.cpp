#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"
#include "wifiiface.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class WifiI : virtual public ::orca::Wifi
{
public:

    WifiI( WifiIface &iface )
        : iface_(iface) {}

    // remote functions

    virtual ::orca::WifiData getData(const Ice::Current&) const
        { return iface_.getData(); }

    virtual void subscribe(const ::orca::WifiConsumerPrx& consumer,
                           const Ice::Current&)
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::WifiConsumerPrx& consumer,
                 const Ice::Current&)
        { iface_.unsubscribe( consumer ); }

private:
    WifiIface &iface_;
};

//////////////////////////////////////////////////////////////////////

WifiIface::WifiIface( const std::string       &ifaceTag, 
                      const orcaice::Context  &context  )
    : ifaceTag_(ifaceTag),
      context_(context)
{
}

WifiIface::~WifiIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
WifiIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::WifiConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new WifiI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}


::orca::WifiData
WifiIface::getData() const
{
    context_.tracer()->debug( "WifiIface::getData()", 5 );

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
WifiIface::subscribe(const ::orca::WifiConsumerPrx& subscriber)
{   
    context_.tracer()->debug( "WifiIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->info( ss.str() );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"WifiIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
WifiIface::unsubscribe(const ::orca::WifiConsumerPrx& subscriber)
{
    context_.tracer()->debug( "WifiIface::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
WifiIface::localSetAndSend( const orca::WifiData& data )
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

