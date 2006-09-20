/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "position3dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;
using namespace segwayrmp;

Position3dI::Position3dI( orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe,
                          const IceStorm::TopicPrx                      & topic )
    : position3dPipe_(position3dPipe),
      topic_(topic)
{
}

// served out the data to the client (it was stored here by the driver at the last read)
orca::Position3dDataPtr
Position3dI::getData(const Ice::Current& current) const
{
    //std::cout << "Sending data back" << std::endl;

    // create a null pointer. data will be cloned into it.
    Position3dDataPtr data;

    try
    {
        position3dPipe_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    //cout << data <<endl;
    return data;
}

orca::Position3dGeometryPtr
Position3dI::getGeometry(const Ice::Current& current) const
{
    //std::cout << "Pretending to send geometry back" << std::endl;

    // @todo implement
    Position3dGeometryPtr geometry = new Position3dGeometry;

    return geometry;
}

void
Position3dI::subscribe(const ::orca::Position3dConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void
Position3dI::unsubscribe(const ::orca::Position3dConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}
