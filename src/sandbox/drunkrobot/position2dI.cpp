/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "position2dI.h"

using namespace std;
using namespace drunk; 

Position2dI::Position2dI( hydroutil::Buffer<orca::Position2dData> &posBuffer, 
                          orca::Position2dGeometry &geomBuffer, 
                          const IceStorm::TopicPrx &topic) :
    posBuffer_(posBuffer), 
    geomBuffer_(geomBuffer), 
    topic_(topic)
{
		
}
	
// served out the data to the client (it was stored here by the driver at the last read)
orca::Position2dData
Position2dI::getData(const Ice::Current& current) const
{
    cout << "Sending data back" << endl;

    // create a null pointer. data will be cloned into it.
    orca::Position2dData data;

    posBuffer_.get( data );
   
    return data; 

}

orca::Position2dGeometry
Position2dI::getGeometry(const Ice::Current& current) const
{
    std::cout << "Sending the geometry we were initialised with" << std::endl;
   
    return geomBuffer_;
}

void 
Position2dI::subscribe(const ::orca::Position2dConsumerPrx& subscriber,
                            const ::Ice::Current&)
{
    cout<<"subscription request"<<endl;
    topic_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
}

void 
Position2dI::unsubscribe(const ::orca::Position2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}
