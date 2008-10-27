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
#include <orcaifaceimpl/util.h>
#include "systemstatusImpl.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//
// This is the implementation of the slice-defined interface
//
class SystemStatusI : public virtual orca::SystemStatus
{
    public:
        SystemStatusI( SystemStatusImpl &impl )
        : impl_(impl) {}

        //
        // Remote calls:
        //

        virtual ::orca::SystemStatusData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

        virtual void subscribe(const ::orca::SystemStatusConsumerPrx &consumer,
                                const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

        virtual void unsubscribe(const ::orca::SystemStatusConsumerPrx& consumer,
                                    const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

    private:
        SystemStatusImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

SystemStatusImpl::SystemStatusImpl( const std::string             &interfaceTag,
                                              const orcaice::Context        &context )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
                     topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
                                context_(context)
{
}

SystemStatusImpl::SystemStatusImpl( const orcaice::Context        &context,
                                              const std::string             &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

SystemStatusImpl::~SystemStatusImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
SystemStatusImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::SystemStatusConsumerPrx>
            ( context_, consumerPrx_, topicName_ );

    // Register with the adapter.
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new SystemStatusI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
SystemStatusImpl::initInterface( gbxiceutilacfr::Thread* thread, 
                                      const std::string& subsysName, 
                                      int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::SystemStatusConsumerPrx>
            ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new SystemStatusI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

orca::SystemStatusData 
SystemStatusImpl::internalGetData() const
{
    context_.tracer().debug( "SystemStatusImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::SystemStatusData data;
    dataStore_.get( data );

    return data;
}


// Subscribe people
void 
SystemStatusImpl::internalSubscribe(const ::orca::SystemStatusConsumerPrx &subscriber)
{
    context_.tracer().debug( "SystemStatusImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

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
        ss <<"SystemStatusImpl::internalSubscribe::failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
SystemStatusImpl::internalUnsubscribe(const ::orca::SystemStatusConsumerPrx &subscriber)
{
    context_.tracer().debug( "SystemStatusImpl::internalUsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
SystemStatusImpl::localSet( const ::orca::SystemStatusData &data )
{
    dataStore_.set( data );
}

void
SystemStatusImpl::localSetAndSend( const ::orca::SystemStatusData &data )
{
    if ( context_.tracer().verbosity( gbxutilacfr::Tracer::DebugTrace, gbxutilacfr::Tracer::ToAny ) >= 5 )
    {
        context_.tracer().debug( "Sending data", 5 );
    }
    dataStore_.set( data );

    // Try to push to IceStorm
    tryPushToIceStormWithReconnect<SystemStatusConsumerPrx,SystemStatusData>( 
            context_,
            consumerPrx_,
            data,
            topicPrx_,
            interfaceName_,
            topicName_ );
}

}

