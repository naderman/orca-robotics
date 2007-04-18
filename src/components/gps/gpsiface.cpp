#if 0
/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "gpsiface.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaifaceimpl/util.h>

using namespace std;

namespace gps {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class GpsI : public orca::Gps
{
public:
    GpsI( GpsIface &iface )
        : iface_(iface) {}

    // remote calls:

    virtual ::orca::GpsData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }
    virtual ::orca::GpsData getMapGridData(const ::Ice::Current& ) const
        { return iface_.getMapGridData(); }
    virtual ::orca::GpsData getTimeData(const ::Ice::Current& ) const
        { return iface_.getTimeData(); }

    virtual void subscribe(const ::orca::GpsConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( subscriber ); }
    virtual void subscribeForMapGrid(const ::orca::GpsConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribeForMapGrid( subscriber ); }
    virtual void subscribeForTime(const ::orca::GpsConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribeForTime( subscriber ); }

    virtual void unsubscribe(const ::orca::GpsConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe( subscriber ); }
    virtual void unsubscribeForMapGrid(const ::orca::GpsConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribeForMapGrid( subscriber ); }
    virtual void unsubscribeForTime(const ::orca::GpsConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribeForTime( subscriber ); }

    virtual orca::GpsDescription getDescription(const Ice::Current& c) const
        { return iface_.getDescription(); }

private:
    GpsIface &iface_;
};

//////////////////////////////////////////////////////////////////////

GpsIface::GpsIface( const std::string &ifaceTag,
                    const orca::GpsDescription &descr,
                    const orcaice::Context &context ) :
    ifaceTag_(ifaceTag),
    description_(descr),
    context_(context)
{
}

GpsIface::~GpsIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
GpsIface::initInterface()
{
    // Find IceStorm Topics to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::GpsConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );
    mapGridTopicPrx_ = orcaice::connectToTopicWithTag<orca::GpsMapGridConsumerPrx>
        ( context_, mapGridConsumerPrx_, ifaceTag_, "mapgrid" );
    timeTopicPrx_ = orcaice::connectToTopicWithTag<orca::GpsTimeConsumerPrx>
        ( context_, timeConsumerPrx_, ifaceTag_, "time" );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new GpsI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

::orca::GpsData 
GpsIface::getData() const
{
    context_.tracer()->debug( "GpsIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::GpsData data;
    dataProxy_.get( data );
    return data;
}

::orca::GpsMapGridData 
GpsIface::getMapGridData() const
{
    context_.tracer()->debug( "GpsIface::getMapGridData()", 5 );

    if ( mapGridDataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::GpsMapGridData data;
    mapGridDataProxy_.get( data );
    return data;
}

::orca::GpsTimeData 
GpsIface::getData() const
{
    context_.tracer()->debug( "GpsIface::getTimeData()", 5 );

    if ( timeDataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::GpsTimeData data;
    timeDataProxy_.get( data );
    return data;
}

void 
GpsIface::subscribe(const ::orca::GpsConsumerPrx& subscriber)
{
    context_.tracer()->debug( "GpsIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"GpsIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
GpsIface::unsubscribe(const ::orca::GpsConsumerPrx& subscriber)
{
    context_.tracer()->debug( "GpsIface::subscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void 
GpsIface::subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx& subscriber)
{
    context_.tracer()->debug( "GpsIface::subscribeForMapGrid(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        mapGridTopicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"GpsIface::subscribeForMapGrid: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
GpsIface::unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx& subscriber)
{
    context_.tracer()->debug( "GpsIface::unsubscribeForMapGrid(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    mapGridTopicPrx_->unsubscribeForMapGrid( subscriber );
}

void 
GpsIface::subscribeForTime(const ::orca::GpsTimeConsumerPrx& subscriber)
{
    context_.tracer()->debug( "GpsIface::subscribeForTime(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        timeTopicPrx_->subscribeForTime( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"GpsIface::subscribeForTime: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
GpsIface::unsubscribeForTime(const ::orca::GpsTimeConsumerPrx& subscriber)
{
    context_.tracer()->debug( "GpsIface::unsubscribeForTime(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    timeTopicPrx_->unsubscribe( subscriber );
}

void
GpsIface::localSetAndSend( const orca::GpsData &data )
{
    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::GpsConsumerPrx,orca::GpsData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

} // namespace
#endif
