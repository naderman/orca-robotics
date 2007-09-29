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
#include "rangescanner2dImpl.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//
// This is the implementation of the slice-defined interface
//
class RangeScanner2dI : public virtual orca::RangeScanner2d
{
public:
    RangeScanner2dI( RangeScanner2dImpl &impl )
        : impl_(impl) {}

    //
    // Remote calls:
    //

    virtual ::orca::RangeScanner2dDataPtr     getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual ::orca::RangeScanner2dDescription getDescription(const ::Ice::Current& ) const
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::RangeScanner2dConsumerPrx &consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::RangeScanner2dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    RangeScanner2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

RangeScanner2dImpl::RangeScanner2dImpl( const orca::RangeScanner2dDescription& descr,
                                  const std::string             &interfaceTag,
                                  const orcaice::Context        &context )
    : descr_(descr),
      interfaceTag_(interfaceTag),
      context_(context)
{
}

RangeScanner2dImpl::~RangeScanner2dImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
RangeScanner2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter.
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new RangeScanner2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

void 
RangeScanner2dImpl::initInterface( orcaiceutil::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_, "*", thread, retryInterval );

    ptr_ = new RangeScanner2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_, thread, retryInterval );
}

orca::RangeScanner2dDataPtr 
RangeScanner2dImpl::internalGetData() const
{
    context_.tracer()->debug( "RangeScanner2dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::RangeScanner2dDataPtr data;
    dataProxy_.get( data );

    return data;
}

// serve out the data to the client (it was stored here earlier by the driver)
orca::RangeScanner2dDescription
RangeScanner2dImpl::internalGetDescription() const
{
    context_.tracer()->debug( "RangeScanner2dImpl::internalGetDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
RangeScanner2dImpl::internalSubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber)
{
    context_.tracer()->debug( "RangeScanner2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

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
        ss <<"RangeScanner2dImpl::internalSubscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
RangeScanner2dImpl::internalUnsubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber)
{
    context_.tracer()->debug( "RangeScanner2dImpl::internalUsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
RangeScanner2dImpl::localSet( const ::orca::RangeScanner2dDataPtr &data )
{
    // cout << "RangeScanner2dImpl::internalSet data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );
}

void
RangeScanner2dImpl::localSetAndSend( const ::orca::RangeScanner2dDataPtr &data )
{
    if ( context_.tracer()->verbosity( orcaiceutil::Tracer::DebugTrace, orcaiceutil::Tracer::ToAny ) >= 5 )
    {
        stringstream ss;
        ss << "RangeScanner2dIface: Sending data: " << orcaice::toString(data);
        context_.tracer()->debug( ss.str(), 5 );
    }
    // cout << "RangeScanner2dImpl::internalSet data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );

    // Try to push to IceStorm
    tryPushToIceStormWithReconnect<RangeScanner2dConsumerPrx,RangeScanner2dDataPtr>( context_,
                                                                                     consumerPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     interfaceTag_ );
}

} // namespace
