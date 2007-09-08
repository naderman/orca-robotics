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
#include <orcaifaceimpl/util.h>
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

    virtual ::orca::PolarFeature2dDataPtr     getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

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

PolarFeature2dImpl::PolarFeature2dImpl( const std::string             &interfaceTag,
                                  const orcaice::Context        &context )
    : interfaceTag_(interfaceTag),
      context_(context)
{
}

PolarFeature2dImpl::~PolarFeature2dImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
PolarFeature2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::PolarFeature2dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PolarFeature2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

orca::PolarFeature2dDataPtr 
PolarFeature2dImpl::internalGetData() const
{
    context_.tracer()->debug( "PolarFeature2dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::PolarFeature2dDataPtr data;
    dataProxy_.get( data );

    return data;
}

// Subscribe people
void 
PolarFeature2dImpl::internalSubscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber)
{
    context_.tracer()->debug( "PolarFeature2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    if ( topicPrx_==0 ) {
        throw orca::SubscriptionFailedException( "null topic proxy." );
    }
    
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
        ss <<"PolarFeature2dImpl::internalSubscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
PolarFeature2dImpl::internalUnsubscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber)
{
    context_.tracer()->debug( "PolarFeature2dImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
PolarFeature2dImpl::localSet( const ::orca::PolarFeature2dDataPtr &data )
{
    // cout << "PolarFeature2dImpl::internalSet data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );
}

void
PolarFeature2dImpl::localSetAndSend( const ::orca::PolarFeature2dDataPtr &data )
{
    if ( context_.tracer()->verbosity( orcaice::Tracer::DebugTrace, orcaice::Tracer::ToAny ) >= 5 )
    {
        stringstream ss;
        ss << "PolarFeature2dIface: Sending data: " << orcaice::toString(data);
        context_.tracer()->debug( ss.str(), 5 );
    }

    // cout << "PolarFeature2dImpl::internalSet data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );

    // Try to push to IceStorm
    tryPushToIceStormWithReconnect<PolarFeature2dConsumerPrx,PolarFeature2dDataPtr>( context_,
                                                                                     consumerPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     interfaceTag_ );
}

} // namespace
