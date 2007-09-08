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
#include "localise3dImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Localise3dI : public orca::Localise3d
{
public:
    Localise3dI( Localise3dImpl &impl )
        : impl_(impl) {}

    // remote calls:

    virtual ::orca::Localise3dData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }
    
    virtual ::orca::VehicleGeometryDescriptionPtr getVehicleGeometry( const ::Ice::Current& ) const
        { return impl_.internalGetVehicleGeometry(); }    

    virtual void subscribe(const ::orca::Localise3dConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::Localise3dConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( subscriber ); }

private:

    Localise3dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

Localise3dImpl::Localise3dImpl( const orca::VehicleGeometryDescriptionPtr &geometry,
                                  const std::string &interfaceTag,
                                  const orcaice::Context &context ) :
    geometry_(geometry),
    interfaceTag_(interfaceTag),
    context_(context)
{    
    stringstream ss;
    ss << "Localise2dImpl::internalGeometry: " << orcaice::toString( geometry );
    context_.tracer()->debug( ss.str(), 5 );
}

Localise3dImpl::~Localise3dImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
Localise3dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Localise3dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Localise3dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

void 
Localise3dImpl::initInterface( orcaice::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Localise3dConsumerPrx>
        ( context_, consumerPrx_, interfaceTag_, "*", thread, retryInterval );

    ptr_ = new Localise3dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_, thread, retryInterval );
}

::orca::Localise3dData 
Localise3dImpl::internalGetData() const
{
    context_.tracer()->debug( "Localise3dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interfaceTag="<<interfaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Localise3dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleGeometryDescriptionPtr
Localise3dImpl::internalGetVehicleGeometry() const
{
    return geometry_;
}

void 
Localise3dImpl::internalSubscribe(const ::orca::Localise3dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise3dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Localise3dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise3dImpl::internalUnsubscribe(const ::orca::Localise3dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise3dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise3dImpl::localSet( const orca::Localise3dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
}

void
Localise3dImpl::localSetAndSend( const orca::Localise3dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Localise3dConsumerPrx,orca::Localise3dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceTag_ );
}

} // namespace
