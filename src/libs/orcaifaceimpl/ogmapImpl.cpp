/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "ogmapImpl.h"
#include <iostream>
#include <orcaice/orcaice.h>
 

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class OgMapI : public orca::OgMap
{
public:

    OgMapI( OgMapImpl &impl )
        : impl_(impl) {}

    // Remote calls:
    orca::OgMapData getData(const Ice::Current&)
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::OgMapConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::OgMapConsumerPrx& consumer,
                             const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

private:
    OgMapImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

OgMapImpl::OgMapImpl( const std::string      &interfaceTag,
                      const orcaice::Context &context ) 
    : interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
      topicName_(orcaice::toTopicAsString(context.name(),interfaceName_)),
      context_(context)      
{
}

OgMapImpl::OgMapImpl( const orcaice::Context &context,
                      const std::string      &interfaceName )                      
    : interfaceName_(interfaceName),
      topicName_(orcaice::toTopicAsString(context.name(),interfaceName_)),
      context_(context)      
{
}

OgMapImpl::~OgMapImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
OgMapImpl::initInterface()
{
    context_.tracer().debug( "OgMapImpl::initInterface()", 5 );

    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::OgMapConsumerPrx>
        ( context_, publisherPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new OgMapI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
OgMapImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    context_.tracer().debug( "OgMapImpl::initInterface(thread)", 5 );

    topicPrx_ = orcaice::connectToTopicWithString<orca::OgMapConsumerPrx>
        ( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new OgMapI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

orca::OgMapData
OgMapImpl::internalGetData() const
{
    context_.tracer().debug( "OgMapImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::OgMapData data;
    dataStore_.get( data );
    return data;
}

void
OgMapImpl::internalSubscribe(const ::orca::OgMapConsumerPrx& subscriber )
{
    context_.tracer().debug( "OgMapImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"OgMapImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
OgMapImpl::internalUnsubscribe(const ::orca::OgMapConsumerPrx& subscriber )
{
    context_.tracer().debug( "OgMapImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void 
OgMapImpl::localSetAndSend( const ::orca::OgMapData &data )
{
    dataStore_.set( data );
    
    // Try to push to IceStorm.
    orcaice::tryPushToIceStormWithReconnect<orca::OgMapConsumerPrx,orca::OgMapData>
        ( context_,
          publisherPrx_,
          data,
          topicPrx_,
          topicName_ );
}

}
