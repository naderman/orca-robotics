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
#include <orcaobj/orcaobj.h>
#include "gpsImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class GpsI : public orca::Gps
{
public:
    GpsI( GpsImpl &impl )
        : impl_(impl) {}
    virtual ~GpsI() {}

    // remote interface

    virtual ::orca::GpsData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual ::orca::GpsDescription getDescription(const ::Ice::Current& ) const
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::GpsConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::GpsConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe(consumer); }

private:
    GpsImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

GpsImpl::GpsImpl( const orca::GpsDescription& descr,
                                const std::string& interfaceTag,
                                const orcaice::Context& context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

GpsImpl::GpsImpl( const orca::GpsDescription& descr,
                                const orcaice::Context& context,
                                const std::string& interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

GpsImpl::~GpsImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
GpsImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::GpsConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new GpsI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
GpsImpl::initInterface( hydroiceutil::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::GpsConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new GpsI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::GpsData 
GpsImpl::internalGetData() const
{
    context_.tracer().debug( "GpsImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::GpsData data;
    dataStore_.get( data );
    return data;
}

::orca::GpsDescription
GpsImpl::internalGetDescription() const
{
    return descr_;
}

void 
GpsImpl::internalSubscribe(const ::orca::GpsConsumerPrx& subscriber )
{
    context_.tracer().debug( "GpsImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"GpsImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
GpsImpl::internalUnsubscribe(const ::orca::GpsConsumerPrx& subscriber)
{
    context_.tracer().debug( "GpsImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
GpsImpl::localSet( const orca::GpsData& data )
{
    dataStore_.set( data );
}

void
GpsImpl::localSetAndSend( const orca::GpsData& data )
{
//     cout<<"TRACE(GpsIface.cpp): localSetAndSend: " << orcaobj::toString(data) << endl;

    dataStore_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::GpsConsumerPrx,orca::GpsData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        interfaceName_,
          topicName_ );
}

} // namespace
