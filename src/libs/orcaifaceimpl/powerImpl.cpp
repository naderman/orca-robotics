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
#include "powerImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PowerI : public orca::Power
{
public:

    PowerI( PowerImpl &impl )
        : impl_(impl) {}
    virtual ~PowerI() {}

    // remote interface

    virtual ::orca::PowerData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::PowerConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::PowerConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    PowerImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PowerImpl::PowerImpl( const std::string& interfaceTag,
                      const orcaice::Context& context )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

PowerImpl::PowerImpl( const orcaice::Context& context,
                      const std::string& interfaceName )                      
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

PowerImpl::~PowerImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
PowerImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::PowerConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PowerI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PowerImpl::initInterface( hydroutil::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::PowerConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, retryInterval );

    ptr_ = new PowerI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, retryInterval );
}

::orca::PowerData 
PowerImpl::internalGetData() const
{
    context_.tracer()->debug( "PowerImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PowerData data;
    dataProxy_.get( data );
    return data;
}

void 
PowerImpl::internalSubscribe(const ::orca::PowerConsumerPrx& subscriber)
{
    context_.tracer()->debug( "PowerImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"PowerImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
PowerImpl::internalUnsubscribe(const ::orca::PowerConsumerPrx& subscriber)
{
    context_.tracer()->debug( "PowerImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
PowerImpl::localSet( const orca::PowerData& data )
{
    dataProxy_.set( data );
}

void
PowerImpl::localSetAndSend( const orca::PowerData& data )
{
//     cout<<"TRACE(PowerIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::PowerConsumerPrx,orca::PowerData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        interfaceName_,
          topicName_ );
}

} // namespace
