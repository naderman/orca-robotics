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
#include "insImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class InsI : public orca::Ins
{
public:
    //! constructor
    InsI( InsImpl &impl )
        : impl_(impl) {}
    virtual ~InsI() {}

    // remote interface

    virtual ::orca::InsData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

    virtual ::orca::InsDescription getDescription(const ::Ice::Current& )
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::InsConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::InsConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe(consumer); }

private:
    InsImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

InsImpl::InsImpl( const orca::InsDescription &descr,
                                const std::string              &interfaceTag,
                                const orcaice::Context         &context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

InsImpl::InsImpl( const orca::InsDescription &descr,
                                const orcaice::Context         &context,
                                const std::string              &interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

InsImpl::~InsImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
InsImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::InsConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new InsI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
InsImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::InsConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new InsI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::InsData 
InsImpl::internalGetData() const
{
    context_.tracer().debug( "InsImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::InsData data;
    dataStore_.get( data );
    return data;
}

::orca::InsDescription
InsImpl::internalGetDescription() const
{
    return descr_;
}

void 
InsImpl::internalSubscribe(const ::orca::InsConsumerPrx& subscriber )
{
    context_.tracer().debug( "InsImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"InsImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
InsImpl::internalUnsubscribe(const ::orca::InsConsumerPrx& subscriber)
{
    context_.tracer().debug( "InsImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
InsImpl::localSet( const orca::InsData& data )
{
    dataStore_.set( data );
}

void
InsImpl::localSetAndSend( const orca::InsData& data )
{
    dataStore_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::InsConsumerPrx,orca::InsData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace
