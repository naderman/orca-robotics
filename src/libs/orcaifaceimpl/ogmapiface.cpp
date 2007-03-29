/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "ogmapiface.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class OgMapI : public orca::OgMap
{
public:

    OgMapI( OgMapIface &iface )
        : iface_(iface) {}

    // Remote calls:
    orca::OgMapData getData(const Ice::Current&) const
        { return iface_.getData(); }

    virtual void subscribe(const ::orca::OgMapConsumerPrx& consumer,
                           const Ice::Current&)
        { iface_.subscribe( consumer ); }

    virtual void unsubscribe(const ::orca::OgMapConsumerPrx& consumer,
                             const Ice::Current&)
        { iface_.unsubscribe( consumer ); }

private:
    OgMapIface &iface_;
};

//////////////////////////////////////////////////////////////////////

OgMapIface::OgMapIface( const std::string      &ifaceTag,
                        const orcaice::Context &context ) 
    : ifaceTag_(ifaceTag),
      context_(context)      
{
}

OgMapIface::~OgMapIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
OgMapIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<OgMapConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new OgMapI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

OgMapData
OgMapIface::getData() const
{
    context_.tracer()->debug( "OgMapIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    OgMapData data;
    dataProxy_.get( data );
    return data;
}

void
OgMapIface::subscribe(const ::OgMapConsumerPrx& subscriber )
{
    context_.tracer()->debug( "OgMapIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"OgMapIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
OgMapIface::unsubscribe(const ::OgMapConsumerPrx& subscriber )
{
    context_.tracer()->debug( "OgMapIface::unsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void 
OgMapIface::localSetAndSend( const ::orca::OgMapData &data )
{
    //cout<<"TRACE(ogmapI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<OgMapConsumerPrx,orca::OgMapData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

}
