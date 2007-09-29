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
#include "cameraImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class CameraI : public orca::Camera
{
public:
    CameraI( CameraImpl &impl )
        : impl_(impl) {}

    // remote interface

    virtual ::orca::CameraData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual ::orca::CameraDescription getDescription(const ::Ice::Current& ) const
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::CameraConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::CameraConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

private:
    CameraImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

CameraImpl::CameraImpl( const orca::CameraDescription& descr,
                          const std::string& interfaceTag,
                          const orcaice::Context& context )
    : descr_(descr),
      interfaceTag_(interfaceTag),
      context_(context)
{
}

CameraImpl::~CameraImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
CameraImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::CameraConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

void 
CameraImpl::initInterface( orcaiceutil::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::CameraConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_, "*", thread, retryInterval );

    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_, thread, retryInterval );
}

::orca::CameraData 
CameraImpl::internalGetData() const
{
    context_.tracer()->debug( "CameraImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::CameraData data;
    dataProxy_.get( data );
    return data;
}

::orca::CameraDescription
CameraImpl::internalGetDescription() const
{
    return descr_;
}

void 
CameraImpl::internalSubscribe(const ::orca::CameraConsumerPrx& subscriber)
{
    context_.tracer()->debug( "CameraImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"CameraImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
CameraImpl::internalUnsubscribe(const ::orca::CameraConsumerPrx& subscriber)
{
    context_.tracer()->debug( "CameraImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
CameraImpl::localSet( const orca::CameraData& data )
{
    dataProxy_.set( data );
}

void
CameraImpl::localSetAndSend( const orca::CameraData& data )
{
//     cout<<"TRACE(CameraIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::CameraConsumerPrx,orca::CameraData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        interfaceTag_ );
}

} // namespace
