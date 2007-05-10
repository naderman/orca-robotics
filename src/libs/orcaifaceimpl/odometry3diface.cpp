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
#include "odometry3diface.h"
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
    Odometry3dI( Odometry3dIface &iface )
        : iface_(iface) {}
    virtual ~Odometry3dI() {}

    // remote interface

    virtual ::orca::Odometry3dData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const
        { return iface_.getDescription(); }

    virtual void subscribe(const ::orca::Odometry3dConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::Odometry3dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe(consumer); }

private:
    Odometry3dIface &iface_;
};

//////////////////////////////////////////////////////////////////////

Odometry3dIface::Odometry3dIface( const orca::VehicleDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    tag_(ifaceTag),
    context_(context)
{
}

Odometry3dIface::~Odometry3dIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
Odometry3dIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry3dConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Odometry3dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, tag_ );
}

::orca::Odometry3dData 
Odometry3dIface::getData() const
{
    context_.tracer()->debug( "Odometry3dIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Odometry3dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleDescription
Odometry3dIface::getDescription() const
{
    return descr_;
}

void 
Odometry3dIface::subscribe(const ::orca::Odometry3dConsumerPrx& subscriber )
{
    context_.tracer()->debug( "Odometry3dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Odometry3dIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Odometry3dIface::unsubscribe(const ::orca::Odometry3dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Odometry3dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Odometry3dIface::localSet( const orca::Odometry3dData& data )
{
    dataProxy_.set( data );
}

void
Odometry3dIface::localSetAndSend( const orca::Odometry3dData& data )
{
//     cout<<"TRACE(Odometry3dIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Odometry3dConsumerPrx,orca::Odometry3dData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
