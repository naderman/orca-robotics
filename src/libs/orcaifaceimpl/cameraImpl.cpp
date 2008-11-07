/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
// #include <orcaifaceimpl/util.h>
#include "cameraImpl.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//
// This is the implementation of the slice-defined interface
//
class CameraI : public virtual orca::Camera
{
public:
    CameraI( CameraImpl &impl )
        : impl_(impl) {}

    //
    // Remote calls:
    //

    virtual ::orca::ImageDataPtr getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }

    virtual ::orca::ImageDescriptionPtr getDescription(const ::Ice::Current& )
        { return impl_.internalGetDescription(); }

    virtual void subscribe(const ::orca::ImageConsumerPrx &consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::ImageConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    CameraImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

CameraImpl::CameraImpl( const orca::CameraDescriptionPtr &descr,
                                        const std::string                     &interfaceTag,
                                        const orcaice::Context                &context )
    : descr_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

CameraImpl::CameraImpl( const orca::CameraDescriptionPtr &descr,
                                        const orcaice::Context                &context,
                                        const std::string                     &interfaceName )
    : descr_(descr),
      interfaceName_(interfaceName),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

CameraImpl::~CameraImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
CameraImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_ );

    // Register with the adapter.
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
CameraImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

orca::ImageDataPtr 
CameraImpl::internalGetData() const
{
    context_.tracer().debug( "CameraImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::CameraDataPtr data;
    dataStore_.get( data );

    return data;
}

// serve out the data to the client (it was stored here earlier by the driver)
orca::ImageDescriptionPtr
CameraImpl::internalGetDescription() const
{
    context_.tracer().debug( "CameraImpl::internalGetDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
CameraImpl::internalSubscribe(const ::orca::ImageConsumerPrx &subscriber)
{
    context_.tracer().debug( "CameraImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

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
        ss <<"CameraImpl::internalSubscribe::failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
CameraImpl::internalUnsubscribe(const ::orca::ImageConsumerPrx &subscriber)
{
    context_.tracer().debug( "CameraImpl::internalUsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
CameraImpl::localSet( const ::orca::CameraDataPtr &data )
{    
    dataStore_.set( data );
}

void
CameraImpl::localSetAndSend( const ::orca::CameraDataPtr &data )
{
    if ( context_.tracer().verbosity( gbxutilacfr::DebugTrace, gbxutilacfr::ToAny ) >= 5 )
    {
        context_.tracer().debug( "Sending data", 5 );
    }
    
    dataStore_.set( data );

    // Try to push to IceStorm
    orcaice::tryPushToIceStormWithReconnect<ImageConsumerPrx,CameraDataPtr>( context_,
                                                                                     publisherPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     topicName_ );
}

} // namespace
