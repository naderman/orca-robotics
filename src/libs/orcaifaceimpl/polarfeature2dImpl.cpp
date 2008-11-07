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
// #include <orcaifaceimpl/util.h>
#include "polarfeature2dImpl.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PolarFeature2dI : public virtual orca::PolarFeature2d
{
public:
    PolarFeature2dI( PolarFeature2dImpl &impl )
        : impl_(impl) {}

    //
    // Remote calls:
    //

    virtual ::orca::PolarFeature2dData     getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

    virtual ::orca::PolarFeature2dDescription getDescription(const ::Ice::Current& )
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::PolarFeature2dConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::PolarFeature2dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    PolarFeature2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PolarFeature2dImpl::PolarFeature2dImpl( const orca::PolarFeature2dDescription&  descr,
                                        const std::string                      &interfaceTag,
                                        const orcaice::Context                 &context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

PolarFeature2dImpl::PolarFeature2dImpl( const orca::PolarFeature2dDescription&  descr,
                                        const orcaice::Context                 &context,
                                        const std::string                      &interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

PolarFeature2dImpl::~PolarFeature2dImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
PolarFeature2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::PolarFeature2dConsumerPrx>
        ( context_, publisherPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PolarFeature2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PolarFeature2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new PolarFeature2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

orca::PolarFeature2dData 
PolarFeature2dImpl::internalGetData() const
{
    context_.tracer().debug( "PolarFeature2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::PolarFeature2dData data;
    dataStore_.get( data );

    return data;
}

// serve out the data to the client (it was stored here earlier by the driver)
orca::PolarFeature2dDescription
PolarFeature2dImpl::internalGetDescription() const
{
    context_.tracer().debug( "PolarFeature2dImpl::internalGetDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
PolarFeature2dImpl::internalSubscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber)
{
    context_.tracer().debug( "PolarFeature2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    if ( topicPrx_==0 ) {
        throw orca::SubscriptionFailedException( "null topic proxy." );
    }
    
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
        ss <<"PolarFeature2dImpl::internalSubscribe::failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
PolarFeature2dImpl::internalUnsubscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber)
{
    context_.tracer().debug( "PolarFeature2dImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
PolarFeature2dImpl::localSet( const ::orca::PolarFeature2dData &data )
{
    dataStore_.set( data );
}

void
PolarFeature2dImpl::localSetAndSend( const ::orca::PolarFeature2dData &data )
{
    if ( context_.tracer().verbosity( gbxutilacfr::DebugTrace, gbxutilacfr::ToAny ) >= 5 )
    {
        context_.tracer().debug( "Sending data", 5 );
    }

    dataStore_.set( data );

    // Try to push to IceStorm
    orcaice::tryPushToIceStormWithReconnect<PolarFeature2dConsumerPrx,PolarFeature2dData>( context_,
                                                                                  publisherPrx_,
                                                                                  data,
                                                                                  topicPrx_,
                                                                                  topicName_ );
}

} // namespace
