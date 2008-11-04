/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include "propertiesImpl.h"
 

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PropertiesI : public orca::Properties
{
public:

    PropertiesI( PropertiesImpl &impl )
        : impl_(impl) {}
    virtual ~PropertiesI() {}

    // remote interface

    virtual ::orca::PropertiesData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

    virtual void setData( const ::orca::PropertiesData &data, const ::Ice::Current& )
        { impl_.internalSetData( data ); }

    virtual void subscribe(const ::orca::PropertiesConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::PropertiesConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    PropertiesImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PropertiesImpl::PropertiesImpl( const std::string& interfaceTag,
                                const orcaice::Context& context )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

PropertiesImpl::PropertiesImpl( const orcaice::Context& context,
                                const std::string& interfaceName )                      
    : interfaceName_(interfaceName),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

PropertiesImpl::~PropertiesImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
PropertiesImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::PropertiesConsumerPrx>
        ( context_, publisherPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PropertiesI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PropertiesImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::PropertiesConsumerPrx>
        ( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new PropertiesI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::PropertiesData 
PropertiesImpl::internalGetData() const
{
    context_.tracer().debug( "PropertiesImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PropertiesData data;
    dataStore_.get( data );
    return data;
}

void
PropertiesImpl::internalSetData( const ::orca::PropertiesData &data )
{
    context_.tracer().debug( "PropertiesImpl::internalSetData()", 5 );

    remotelySetDataStore_.set( data );
}

void 
PropertiesImpl::internalSubscribe(const ::orca::PropertiesConsumerPrx& subscriber)
{
    context_.tracer().debug( "PropertiesImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"PropertiesImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
PropertiesImpl::internalUnsubscribe(const ::orca::PropertiesConsumerPrx& subscriber)
{
    context_.tracer().debug( "PropertiesImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
PropertiesImpl::localSet( const orca::PropertiesData& data )
{
    dataStore_.set( data );
}

void
PropertiesImpl::localSetAndSend( const orca::PropertiesData& data )
{
    dataStore_.set( data );

    // Try to push to IceStorm.
    orcaice::tryPushToIceStormWithReconnect<orca::PropertiesConsumerPrx,orca::PropertiesData>
        ( context_,
          publisherPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace
