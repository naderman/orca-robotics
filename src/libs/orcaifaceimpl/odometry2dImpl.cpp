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
#include "odometry2dImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Odometry2dI : public orca::Odometry2d
{
public:
    Odometry2dI( Odometry2dImpl &impl )
        : impl_(impl) {}
    virtual ~Odometry2dI() {}

    // remote interface

    virtual ::orca::Odometry2dData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::Odometry2dConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::Odometry2dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe(consumer); }

private:
    Odometry2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

Odometry2dImpl::Odometry2dImpl( const orca::VehicleDescription& descr,
                          const std::string& interfaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    interfaceTag_(interfaceTag),
    context_(context)
{
}

Odometry2dImpl::~Odometry2dImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
Odometry2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry2dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Odometry2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

void 
Odometry2dImpl::initInterface( hydroutil::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry2dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_, "*", thread, retryInterval );

    ptr_ = new Odometry2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_, thread, retryInterval );
}

::orca::Odometry2dData 
Odometry2dImpl::internalGetData() const
{
    context_.tracer()->debug( "Odometry2dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Odometry2dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleDescription
Odometry2dImpl::internalGetDescription() const
{
    return descr_;
}

void 
Odometry2dImpl::internalSubscribe(const ::orca::Odometry2dConsumerPrx& subscriber )
{
    context_.tracer()->debug( "Odometry2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Odometry2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Odometry2dImpl::internalUnsubscribe(const ::orca::Odometry2dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Odometry2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Odometry2dImpl::localSet( const orca::Odometry2dData& data )
{
    dataProxy_.set( data );
}

void
Odometry2dImpl::localSetAndSend( const orca::Odometry2dData& data )
{
//     cout<<"TRACE(Odometry2dIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Odometry2dConsumerPrx,orca::Odometry2dData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        interfaceTag_ );
}

} // namespace
