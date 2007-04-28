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
#include "poweriface.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PowerI : public orca::Power
{
public:

    PowerI( PowerIface &iface )
        : iface_(iface) {}
    virtual ~PowerI() {}

    // remote interface

    virtual ::orca::PowerData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual void subscribe(const ::orca::PowerConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::PowerConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe( consumer ); }

private:
    PowerIface &iface_;
};

//////////////////////////////////////////////////////////////////////

PowerIface::PowerIface( const std::string& ifaceTag,
                const orcaice::Context& context ) :
    tag_(ifaceTag),
    context_(context)
{
}

PowerIface::~PowerIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
PowerIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::PowerConsumerPrx>
        ( context_, consumerPrx_, tag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PowerI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, tag_ );
}

::orca::PowerData 
PowerIface::getData() const
{
    context_.tracer()->debug( "PowerIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<tag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PowerData data;
    dataProxy_.get( data );
    return data;
}

void 
PowerIface::subscribe(const ::orca::PowerConsumerPrx& subscriber)
{
    context_.tracer()->debug( "PowerIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"PowerIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
PowerIface::unsubscribe(const ::orca::PowerConsumerPrx& subscriber)
{
    context_.tracer()->debug( "PowerIface::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
PowerIface::localSet( const orca::PowerData& data )
{
    dataProxy_.set( data );
}

void
PowerIface::localSetAndSend( const orca::PowerData& data )
{
//     cout<<"TRACE(PowerIface.cpp): localSetAndSend: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );

    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::PowerConsumerPrx,orca::PowerData>
        ( context_,
        consumerPrx_,
        data,
        topicPrx_,
        tag_ );
}

} // namespace
