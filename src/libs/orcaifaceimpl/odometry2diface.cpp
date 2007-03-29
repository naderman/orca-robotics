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
#include "odometry2diface.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Odometry2dI : public orca::Odometry2d
{
public:
    //! constructor
    Odometry2dI( Odometry2dIface &iface )
        : iface_(iface) {}
    virtual ~Odometry2dI() {}

    // remote interface

    virtual ::orca::Odometry2dData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const
        { return iface_.getDescription(); }

    virtual void subscribe(const ::orca::Odometry2dConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::Odometry2dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe(consumer); }

private:
    Odometry2dIface &iface_;
};

//////////////////////////////////////////////////////////////////////

Odometry2dIface::Odometry2dIface( const orca::VehicleDescription& descr,
                          const std::string& ifaceTag,
                          const orcaice::Context& context ) :
    descr_(descr),
    tag_(ifaceTag),
    context_(context)
{
}

Odometry2dIface::~Odometry2dIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
Odometry2dIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Odometry2dConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Odometry2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, tag_ );
}

::orca::Odometry2dData 
Odometry2dIface::getData() const
{
    context_.tracer()->debug( "Odometry2dIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Odometry2dData data;
    dataProxy_.get( data );
    return data;
}

::orca::VehicleDescription
Odometry2dIface::getDescription() const
{
    return descr_;
}

void 
Odometry2dIface::subscribe(const ::orca::Odometry2dConsumerPrx& subscriber )
{
    context_.tracer()->debug( "Odometry2dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Odometry2dIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Odometry2dIface::unsubscribe(const ::orca::Odometry2dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Odometry2dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Odometry2dIface::localSet( const orca::Odometry2dData& data )
{
    dataProxy_.set( data );
}

void
Odometry2dIface::localSetAndSend( const orca::Odometry2dData& data )
{
//     cout<<"TRACE(Odometry2dIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Odometry2dConsumerPrx,orca::Odometry2dData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
