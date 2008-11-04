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
#include "laserscanner2dImpl.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//
// This is the implementation of the slice-defined interface
//
class LaserScanner2dI : public virtual orca::LaserScanner2d
{
public:
    LaserScanner2dI( LaserScanner2dImpl &impl )
        : impl_(impl) {}

    //
    // Remote calls:
    //

    virtual ::orca::RangeScanner2dDataPtr     getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

    virtual ::orca::RangeScanner2dDescription getDescription(const ::Ice::Current& )
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::RangeScanner2dConsumerPrx &consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::RangeScanner2dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    LaserScanner2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

LaserScanner2dImpl::LaserScanner2dImpl( const orca::RangeScanner2dDescription &descr,
                                        const std::string                     &interfaceTag,
                                        const orcaice::Context                &context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

LaserScanner2dImpl::LaserScanner2dImpl( const orca::RangeScanner2dDescription &descr,
                                        const orcaice::Context                &context,
                                        const std::string                     &interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

LaserScanner2dImpl::~LaserScanner2dImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
LaserScanner2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_ );

    // Register with the adapter.
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new LaserScanner2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
LaserScanner2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new LaserScanner2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

orca::RangeScanner2dDataPtr 
LaserScanner2dImpl::internalGetData() const
{
    context_.tracer().debug( "LaserScanner2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::LaserScanner2dDataPtr data;
    dataStore_.get( data );

    return data;
}

// serve out the data to the client (it was stored here earlier by the driver)
orca::RangeScanner2dDescription
LaserScanner2dImpl::internalGetDescription() const
{
    context_.tracer().debug( "LaserScanner2dImpl::internalGetDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
LaserScanner2dImpl::internalSubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber)
{
    context_.tracer().debug( "LaserScanner2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

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
        ss <<"LaserScanner2dImpl::internalSubscribe::failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
LaserScanner2dImpl::internalUnsubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber)
{
    context_.tracer().debug( "LaserScanner2dImpl::internalUsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
LaserScanner2dImpl::localSet( const ::orca::LaserScanner2dDataPtr &data )
{
    dataStore_.set( data );
}

void
LaserScanner2dImpl::localSetAndSend( const ::orca::LaserScanner2dDataPtr &data )
{
    if ( context_.tracer().verbosity( gbxutilacfr::Tracer::DebugTrace, gbxutilacfr::Tracer::ToAny ) >= 5 )
    {
        context_.tracer().debug( "Sending data", 5 );
    }
    
    dataStore_.set( data );

    // Try to push to IceStorm
    orcaice::tryPushToIceStormWithReconnect<RangeScanner2dConsumerPrx,LaserScanner2dDataPtr>( context_,
                                                                                     publisherPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     interfaceName_,
                                                                                     topicName_ );
}

} // namespace
