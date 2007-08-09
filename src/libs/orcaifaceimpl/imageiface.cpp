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
#include "imageiface.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class ImageI : public orca::Image
{
public:
    ImageI( ImageIface &iface )
        : iface_(iface) {}

    // remote interface

    virtual ::orca::ImageDataPtr getData(const ::Ice::Current& )
        { return iface_.getData(); }

    virtual ::orca::ImageDescription getDescription(const ::Ice::Current& )
        { return iface_.getDescription(); }

    virtual void subscribe(const ::orca::ImageConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::ImageConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

private:
    ImageIface &iface_;
};

//////////////////////////////////////////////////////////////////////

ImageIface::ImageIface( const orca::ImageDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context )
    : descr_(descr),
      ifaceTag_(ifaceTag),
      context_(context)
{
}

ImageIface::~ImageIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
ImageIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::ImageConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new ImageI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

void 
ImageIface::initInterface( orcaice::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::ImageConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_, "*", thread, retryInterval );

    ptr_ = new ImageI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_, thread, retryInterval );
}

::orca::ImageDataPtr 
ImageIface::getData() const
{
    context_.tracer()->debug( "ImageIface::getData()", 10 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::ImageDataPtr data;
    dataProxy_.get( data );
    return data;
}

::orca::ImageDescription
ImageIface::getDescription() const
{
    return descr_;
}

void 
ImageIface::subscribe(const ::orca::ImageConsumerPrx& subscriber)
{
    context_.tracer()->debug( "ImageIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"ImageIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
ImageIface::unsubscribe(const ::orca::ImageConsumerPrx& subscriber)
{
    context_.tracer()->debug( "ImageIface::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
ImageIface::localSet( const orca::ImageDataPtr& data )
{
    dataProxy_.set( data );
}

void
ImageIface::localSetAndSend( const orca::ImageDataPtr& data )
{
//     cout<<"TRACE(ImageIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;
    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::ImageConsumerPrx,orca::ImageDataPtr>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        ifaceTag_ );
}

} // namespace
