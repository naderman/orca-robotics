/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "drivebicycleiface.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class DriveBicycleI : public orca::DriveBicycle
{
public:
    DriveBicycleI( DriveBicycleIface &iface )
        : iface_(iface) {}

    // remote calls:

    virtual ::orca::DriveBicycleData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual void subscribe(const ::orca::DriveBicycleConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::DriveBicycleConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe( subscriber ); }

    virtual orca::VehicleDescription getDescription( const Ice::Current& c ) const
        { return iface_.getDescription(); }

    virtual void setCommand(const ::orca::DriveBicycleData& command, 
                            const ::Ice::Current& current )
        {  iface_.setCommand( command ); }

private:
    DriveBicycleIface &iface_;
};

//////////////////////////////////////////////////////////////////////

DriveBicycleIface::DriveBicycleIface( const std::string &ifaceTag,
                                      orcaice::Proxy<orca::DriveBicycleData>& commandProxy,
                                      const orca::VehicleDescription& descr,
                                      const orcaice::Context &context ) :
    ifaceTag_(ifaceTag),
    commandProxy_(commandProxy),
    description_(descr),
    context_(context)
{
}

DriveBicycleIface::~DriveBicycleIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
DriveBicycleIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::DriveBicycleConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new DriveBicycleI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

::orca::DriveBicycleData 
DriveBicycleIface::getData() const
{
    context_.tracer()->debug( "DriveBicycleIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::DriveBicycleData data;
    dataProxy_.get( data );
    return data;
}

void
DriveBicycleIface::setCommand(const ::orca::DriveBicycleData& command )
{
    commandProxy_.set( command );
}

void 
DriveBicycleIface::subscribe(const ::orca::DriveBicycleConsumerPrx& subscriber)
{
    context_.tracer()->debug( "DriveBicycleIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"DriveBicycleIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
DriveBicycleIface::unsubscribe(const ::orca::DriveBicycleConsumerPrx& subscriber)
{
    context_.tracer()->debug( "DriveBicycleIface::subscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
DriveBicycleIface::localSet( const orca::DriveBicycleData &data )
{
    cout<<"TRACE(drivebicycleIface.cpp): localSet(): " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
}

void
DriveBicycleIface::localSetAndSend( const orca::DriveBicycleData &data )
{
    cout<<"TRACE(drivebicycleiface.cpp): localSetAndSend(): " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::DriveBicycleConsumerPrx,orca::DriveBicycleData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

} // namespace