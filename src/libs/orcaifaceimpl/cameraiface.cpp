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
#include "cameraiface.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class CameraI : public orca::Camera
{
public:
    //! constructor
    CameraI( CameraIface &iface )
        : iface_(iface) {}

    // remote interface

    virtual ::orca::CameraData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual ::orca::CameraDescription getDescription(const ::Ice::Current& ) const
        { return iface_.getDescription(); }

    virtual void subscribe(const ::orca::CameraConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::CameraConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

private:
    CameraIface &iface_;
};

//////////////////////////////////////////////////////////////////////

CameraIface::CameraIface( const orca::CameraDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context )
    : descr_(descr),
      tag_(ifaceTag),
      context_(context)
{
}

CameraIface::~CameraIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
CameraIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::CameraConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new CameraI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, tag_ );
}

::orca::CameraData 
CameraIface::getData() const
{
    context_.tracer()->debug( "CameraIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::CameraData data;
    dataProxy_.get( data );
    return data;
}

::orca::CameraDescription
CameraIface::getDescription() const
{
    return descr_;
}

void 
CameraIface::subscribe(const ::orca::CameraConsumerPrx& subscriber)
{
    context_.tracer()->debug( "CameraIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"CameraIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
CameraIface::unsubscribe(const ::orca::CameraConsumerPrx& subscriber)
{
    context_.tracer()->debug( "CameraIface::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
CameraIface::localSet( const orca::CameraData& data )
{
    dataProxy_.set( data );
}

void
CameraIface::localSetAndSend( const orca::CameraData& data )
{
//     cout<<"TRACE(CameraIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::CameraConsumerPrx,orca::CameraData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
