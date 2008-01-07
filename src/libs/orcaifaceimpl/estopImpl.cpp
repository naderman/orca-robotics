/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <hydroiceutil/store.h>

#include <orca/estop.h>
#include <orcaice/orcaice.h>
#include "estopImpl.h"
#include "util.h"

using namespace std;

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
namespace orcaifaceimpl{
class EStopI : public orca::EStop
{
public:

    EStopI( EStopImpl &impl )
        : impl_(impl) {};
    virtual ~EStopI() {};

    // remote interface
    virtual ::orca::EStopData getData(const ::Ice::Current& ) const
        { return impl_.internalGet(); }

    virtual void setData(const orca::EStopData& data, const Ice::Current& current)
        { impl_.localSet(data); }

    virtual void subscribe(const ::orca::EStopConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::EStopConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    EStopImpl &impl_;
};

} // namespace

using namespace orcaifaceimpl;


//////////////////////////////////////////////////////////////////////

EStopImpl::EStopImpl( const std::string& interfaceTag,
                      const orcaice::Context& context )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

EStopImpl::EStopImpl( const orcaice::Context& context,
                      const std::string& interfaceName )                      
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

EStopImpl::~EStopImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}


void
EStopImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::EStopConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new EStopI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}


void 
EStopImpl::initInterface( hydroiceutil::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::EStopConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new EStopI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}


orca::EStopData
EStopImpl::internalGet() const
{
    context_.tracer().debug( "EStopImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }
    
    // Get the most recent 
    orca::EStopData data;
    dataStore_.get( data );

    return data;
}


void 
EStopImpl::internalSubscribe(const ::orca::EStopConsumerPrx& subscriber)
{
    context_.tracer().debug( "EStopImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"EStopImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
EStopImpl::internalUnsubscribe(const ::orca::EStopConsumerPrx& subscriber)
{
    context_.tracer().debug( "EStopImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}


void
EStopImpl::localSet( const orca::EStopData data )
{
    dataStore_.set( data );
}


void
EStopImpl::localSetAndSend(const orca::EStopData data )
{
//     cout<<"TRACE(EStopIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataStore_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::EStopConsumerPrx, ::orca::EStopData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        interfaceName_,
          topicName_ );
}

