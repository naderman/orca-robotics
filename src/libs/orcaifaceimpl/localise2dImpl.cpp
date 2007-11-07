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
#include <orcaice/orcaice.h>
#include "localise2dImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Localise2dI : public orca::Localise2d
{
public:
    Localise2dI( Localise2dImpl &impl )
        : impl_(impl) {}

    // remote calls:

    virtual ::orca::Localise2dData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }
    
    virtual ::orca::VehicleGeometryDescriptionPtr getVehicleGeometry( const ::Ice::Current& ) const
        { return impl_.internalGetVehicleGeometry(); }    

    virtual void subscribe(const ::orca::Localise2dConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( subscriber ); }

private:
    Localise2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

Localise2dImpl::Localise2dImpl( const orca::VehicleGeometryDescriptionPtr &geometry,
                                const std::string &interfaceTag,
                                const orcaice::Context &context )
    : geometry_(geometry),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

Localise2dImpl::Localise2dImpl( const orca::VehicleGeometryDescriptionPtr &geometry,
                                const orcaice::Context &context,
                                const std::string &interfaceName )
    : geometry_(geometry),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

Localise2dImpl::~Localise2dImpl()
{
    cout<<"TRACE(localise2dImpl.cpp): interfaceName_: " << interfaceName_ << endl;
    cout<<"TRACE(localise2dImpl.cpp): ice_id(): " << ptr_->ice_id() << endl;

    tryRemoveInterface( context_, interfaceName_ );

//     try {
//         // context.adapter()->remove( context.communicator()->stringToIdentity( ptr->ice_id() ) );
//         context_.adapter()->remove( context_.communicator()->stringToIdentity( interfaceName_ ) );
//     }
//     catch ( Ice::ObjectAdapterDeactivatedException & )
//     {
//         // This is OK, we're shutting down.
//     }
//     catch ( Ice::Exception &e )
//     {
//         cout << "orcaifaceimpl: Exception when removing Ice::ObjectPtr with id "<<ptr_->ice_id()<<" from adapter: " << e << endl;
//     }
}

void
Localise2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::Localise2dConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Localise2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
Localise2dImpl::initInterface( hydroutil::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::Localise2dConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, retryInterval );

    ptr_ = new Localise2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, retryInterval );
}

::orca::Localise2dData 
Localise2dImpl::internalGetData() const
{
    context_.tracer()->debug( "Localise2dImpl::internalGetData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Localise2dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleGeometryDescriptionPtr
Localise2dImpl::internalGetVehicleGeometry() const
{
    return geometry_;
}

void 
Localise2dImpl::internalSubscribe(const ::orca::Localise2dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Localise2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise2dImpl::internalUnsubscribe(const ::orca::Localise2dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise2dImpl::internalUnsubscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise2dImpl::localSet( const orca::Localise2dData &data )
{
    //cout<<"TRACE(localise2dIface.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
}

void
Localise2dImpl::localSetAndSend( const orca::Localise2dData &data )
{
    //cout<<"TRACE(localise2dIface.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Localise2dConsumerPrx,orca::Localise2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace
