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
#include <orcaifaceimpl/util.h>
#include "laserscanner2diface.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//
// This is the implementation of the slice-defined interface
//
class LaserScanner2dI : public virtual orca::LaserScanner2d
{
public:
    LaserScanner2dI( LaserScanner2dIface &iface )
        : iface_(iface) {}

    //
    // Remote calls:
    //

    virtual ::orca::RangeScanner2dDataPtr     getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual ::orca::RangeScanner2dDescription getDescription(const ::Ice::Current& ) const
        { return iface_.getDescription(); }

    virtual void subscribe(const ::orca::RangeScanner2dConsumerPrx &consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::RangeScanner2dConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe( consumer ); }

private:
    LaserScanner2dIface &iface_;
};

//////////////////////////////////////////////////////////////////////

LaserScanner2dIface::LaserScanner2dIface( const orca::RangeScanner2dDescription& descr,
                                  const std::string             &ifaceTag,
                                  const orcaice::Context        &context )
    : descr_(descr),
      ifaceTag_(ifaceTag),
      context_(context)
{
}

LaserScanner2dIface::~LaserScanner2dIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
LaserScanner2dIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter.
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new LaserScanner2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

void 
LaserScanner2dIface::initInterface( orcaice::Thread* thread, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_, "*", thread, retryInterval );

    ptr_ = new LaserScanner2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_, thread, retryInterval );
}

orca::RangeScanner2dDataPtr 
LaserScanner2dIface::getData() const
{
    context_.tracer()->debug( "LaserScanner2dIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    // create a null pointer. data will be cloned into it.
    orca::LaserScanner2dDataPtr data;
    dataProxy_.get( data );

    return data;
}

// serve out the data to the client (it was stored here earlier by the driver)
orca::RangeScanner2dDescription
LaserScanner2dIface::getDescription() const
{
    context_.tracer()->debug( "LaserScanner2dIface::getDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
LaserScanner2dIface::subscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber)
{
    context_.tracer()->debug( "LaserScanner2dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    if ( topicPrx_==0 ) {
        throw orca::SubscriptionFailedException( "null topic proxy." );
    }
    
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
        ss <<"LaserScanner2dIface::subscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
LaserScanner2dIface::unsubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber)
{
    context_.tracer()->debug( "LaserScanner2dIface::usubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );

    topicPrx_->unsubscribe( subscriber );
}

void
LaserScanner2dIface::localSet( const ::orca::LaserScanner2dDataPtr &data )
{
    // cout << "LaserScanner2dIface::set data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );
}

void
LaserScanner2dIface::localSetAndSend( const ::orca::LaserScanner2dDataPtr &data )
{
    if ( context_.tracer()->verbosity( orcaice::Tracer::DebugTrace, orcaice::Tracer::ToAny ) >= 5 )
    {
        stringstream ss;
        ss << "LaserScanner2dIface: Sending data: " << orcaice::toString(data);
        context_.tracer()->debug( ss.str(), 5 );
    }
    // cout << "LaserScanner2dIface::set data: " << orcaice::toString( data ) << endl;
    
    dataProxy_.set( data );

    // Try to push to IceStorm
    tryPushToIceStormWithReconnect<RangeScanner2dConsumerPrx,LaserScanner2dDataPtr>( context_,
                                                                                     consumerPrx_,
                                                                                     data,
                                                                                     topicPrx_,
                                                                                     ifaceTag_ );
}

} // namespace
