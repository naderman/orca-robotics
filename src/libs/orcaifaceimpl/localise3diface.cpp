/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "localise3diface.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Localise3dI : public orca::Localise3d
{
public:
    Localise3dI( Localise3dIface &iface )
        : iface_(iface) {}

    // remote calls:

    virtual ::orca::Localise3dData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual void subscribe(const ::orca::Localise3dConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::Localise3dConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe( subscriber ); }

    virtual orca::Localise3dData getDataAtTime(const orca::Time& t,
                                               const Ice::Current& c) const
        { return iface_.getDataAtTime(t); }

private:

    Localise3dIface &iface_;
};

//////////////////////////////////////////////////////////////////////

Localise3dIface::Localise3dIface( const std::string &ifaceTag,
                          const orcaice::Context &context ) :
    ifaceTag_(ifaceTag),
    context_(context)
{
}

Localise3dIface::~Localise3dIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
Localise3dIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Localise3dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Localise3dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

::orca::Localise3dData 
Localise3dIface::getData() const
{
    context_.tracer()->debug( "Localise3dIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Localise3dData data;
    dataProxy_.get( data );
    return data;
}

void 
Localise3dIface::subscribe(const ::orca::Localise3dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise3dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Localise3dIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise3dIface::unsubscribe(const ::orca::Localise3dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise3dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise3dIface::localSet( const orca::Localise3dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
}

void
Localise3dIface::localSetAndSend( const orca::Localise3dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Localise3dConsumerPrx,orca::Localise3dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

} // namespace
