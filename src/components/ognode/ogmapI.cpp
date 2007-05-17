/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "ogmapI.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using namespace orcaice;
using namespace ognode;

OgMapI::OgMapI( const orca::OgMapData& theMap,
                const std::string  &tag,
                const orcaice::Context & context )
    : context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<OgMapConsumerPrx>
        ( context_, consumerPrx_, tag );

    // Stick it in the buffer so pullers can get it
    ogMapDataBuffer_.push( theMap );
}

OgMapData
OgMapI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    if ( ogMapDataBuffer_.isEmpty() )
	throw orca::DataNotExistException("No Map");

    orca::OgMapData data;
    ogMapDataBuffer_.get(data);
    return data;
}

void
OgMapI::localSetData( const OgMapData& data )
{
    // Stick it in the buffer so pullers can get it
    ogMapDataBuffer_.push( data );

    // Try to push the map out to IceStorm
    try {
        context_.tracer()->debug( "Pushing to IceStorm", 2 );
        consumerPrx_->setData( data );
        context_.tracer()->debug( "Done Push to IceStorm", 3 );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

void
OgMapI::subscribe(const ::OgMapConsumerPrx& subscriber,
                  const Ice::Current& current)
{
    cout<<"subscribe()"<<endl;
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
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
OgMapI::unsubscribe(const ::OgMapConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}
