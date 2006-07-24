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
#include "position2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

Position2dI::Position2dI( PtrBuffer<Position2dDataPtr> &posBuffer, Position2dGeometryPtr &geomBuffer, const IceStorm::TopicPrx &topic) :
  posBuffer_(posBuffer), geomBuffer_(geomBuffer), topic_(topic)
{
		
}
	
// served out the data to the client (it was stored here by the driver at the last read)
orca::Position2dDataPtr Position2dI::getData(const Ice::Current& current) const
{
    std::cout << "Sending data back" << std::endl;

    // create a null pointer. data will be cloned into it.
    Position2dDataPtr data;

    posBuffer_.get( data );
   
    return data; 

}

orca::Position2dGeometryPtr Position2dI::getGeometry(const Ice::Current& current) const
{
    std::cout << "Sending the geometry we were initialised with" << std::endl;
   
    return geomBuffer_;
}

void Position2dI::subscribe(const ::orca::Position2dConsumerPrx& subscriber,
                            const ::Ice::Current&)
{
    cout<<"subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void Position2dI::unsubscribe(const ::orca::Position2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}
