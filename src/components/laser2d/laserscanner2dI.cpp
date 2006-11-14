/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "sicklaserutil.h"

#include "laserscanner2dI.h"

using namespace std;
using namespace orca;

namespace laser2d {

LaserScanner2dI::LaserScanner2dI( orca::RangeScanner2dDescriptionPtr  descr,
                                  const std::string             &ifaceTag,
                                  const orcaice::Context        &context )
    : descr_(descr),
      ifaceTag_(ifaceTag),
      context_(context)
{
    // Find IceStorm Topic to which we'll publish
    // this will throw NetworkException if can't connect
    topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );
}

orca::RangeScanner2dDataPtr 
LaserScanner2dI::getData(const Ice::Current& current) const
{
    context_.tracer()->debug( "getData()", 5 );

    // we don't need to pop the data here because we don't block on it.
    if ( dataBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    // create a null pointer. data will be cloned into it.
    orca::LaserScanner2dDataPtr data;
    dataBuffer_.get( data );

    return data;
}

// served out the data to the client (it was stored here earlier by the driver)
orca::RangeScanner2dDescriptionPtr 
LaserScanner2dI::getDescription(const Ice::Current& current) const
{
    context_.tracer()->debug( "getDescription()", 5 );
    return descr_;
}

// Subscribe people
void 
LaserScanner2dI::subscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "subscribe()", 5 );

    if ( topicPrx_==0 ) {
        throw orca::SubscriptionFailedException( "null topic proxy." );
    }
    
    IceStorm::QoS qos;
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"laserI::subscribe::failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( "failed to subscribe to an IceStorm topic." );
    }
}

// Unsubscribe people
void 
LaserScanner2dI::unsubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "unsubscribe()", 5 );

    topicPrx_->unsubscribe( subscriber );
}

void
LaserScanner2dI::localSetData( const ::orca::LaserScanner2dDataPtr data )
{
    // debug
    //cout << orcaice::toVerboseString( data ) << endl;
    
    dataBuffer_.push( data );

    // Try to push it out to IceStorm too
    try {
        consumerPrx_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        std::stringstream ss; ss << "Failed push to IceStorm: " << e;
        context_.tracer()->warning( ss.str() );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer()->info( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::RangeScanner2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer()->print( "Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setData( data );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer()->print( "Re-connection to IceStorm failed." );
        }
    }
}

} // namespace
