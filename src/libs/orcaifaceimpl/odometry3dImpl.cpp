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
#include "odometry3dImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Odometry3dI : public orca::Odometry3d
{
public:
    //! constructor
    Odometry3dI( Odometry3dImpl &impl )
        : impl_(impl) {}
    virtual ~Odometry3dI() {}

    // remote interface

    virtual ::orca::Odometry3dData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::Odometry3dConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::Odometry3dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe(consumer); }

private:
    Odometry3dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

Odometry3dImpl::Odometry3dImpl( const orca::VehicleDescription& descr,
                          const std::string& interfaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    interfaceTag_(interfaceTag),
    context_(context)
{
}

Odometry3dImpl::~Odometry3dImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
Odometry3dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry3dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Odometry3dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

void 
Odometry3dImpl::initInterface( orcaiceutil::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry3dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_, "*", thread, retryInterval );

    ptr_ = new Odometry3dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_, thread, retryInterval );
}

::orca::Odometry3dData 
Odometry3dImpl::internalGetData() const
{
    context_.tracer()->debug( "Odometry3dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Odometry3dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleDescription
Odometry3dImpl::internalGetDescription() const
{
    return descr_;
}

void 
Odometry3dImpl::internalSubscribe(const ::orca::Odometry3dConsumerPrx& subscriber )
{
    context_.tracer()->debug( "Odometry3dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Odometry3dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Odometry3dImpl::internalUnsubscribe(const ::orca::Odometry3dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Odometry3dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Odometry3dImpl::localSet( const orca::Odometry3dData& data )
{
    dataProxy_.set( data );
}

void
Odometry3dImpl::localSetAndSend( const orca::Odometry3dData& data )
{
//     cout<<"TRACE(Odometry3dIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Odometry3dConsumerPrx,orca::Odometry3dData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        interfaceTag_ );
}

} // namespace
