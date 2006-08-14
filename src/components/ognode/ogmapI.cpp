/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

OgMapI::OgMapI( orca::OgMapDataPtr  theMap,
                const std::string  &tag,
                orcaice::Context    context )
    : context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<OgMapConsumerPrx>
        ( context_, consumerPrx_, tag );

    // Stick it in the buffer so pullers can get it
    ogMapDataBuffer_.push( theMap );
}

OgMapDataPtr
OgMapI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    if ( ogMapDataBuffer_.isEmpty() )
	throw orca::DataNotExistException("No Map");

    orca::OgMapDataPtr data;
    ogMapDataBuffer_.get(data);
    return data;
}

void
OgMapI::localSetData(OgMapDataPtr data)
{
    // Stick it in the buffer so pullers can get it
    ogMapDataBuffer_.push( data );

    // Try to push the map out to IceStorm
    try {
        context_.tracer()->debug( "Pushing to IceStorm", 2 );
        consumerPrx_->setData( data );
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
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

void
OgMapI::unsubscribe(const ::OgMapConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}
