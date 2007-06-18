/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "localise2diface.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Localise2dI : public orca::Localise2d
{
public:
    Localise2dI( Localise2dIface &iface )
        : iface_(iface) {}

    // remote calls:

    virtual ::orca::Localise2dData getData(const ::Ice::Current& ) const
        { return iface_.getData(); }

    virtual void subscribe(const ::orca::Localise2dConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { iface_.subscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { iface_.unsubscribe( subscriber ); }

    virtual orca::Localise2dData getDataAtTime(const orca::Time& time, 
                                               const Ice::Current& c) const
        { return iface_.getDataAtTime( time ); }

private:
    Localise2dIface &iface_;
};

//////////////////////////////////////////////////////////////////////

Localise2dIface::Localise2dIface( const std::string &ifaceTag,
                                  const orcaice::Context &context ) :
    ifaceTag_(ifaceTag),
    context_(context)
{
}

Localise2dIface::~Localise2dIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
Localise2dIface::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Localise2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Localise2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

::orca::Localise2dData 
Localise2dIface::getData() const
{
    context_.tracer()->debug( "Localise2dIface::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Localise2dData data;
    dataProxy_.get( data );
    return data;
}

void 
Localise2dIface::subscribe(const ::orca::Localise2dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise2dIface::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Localise2dIface::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise2dIface::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber)
{
    context_.tracer()->debug( "Localise2dIface::unsubscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise2dIface::localSet( const orca::Localise2dData &data )
{
    //cout<<"TRACE(localise2dIface.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
}

void
Localise2dIface::localSetAndSend( const orca::Localise2dData &data )
{
    //cout<<"TRACE(localise2dIface.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Localise2dConsumerPrx,orca::Localise2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

} // namespace
