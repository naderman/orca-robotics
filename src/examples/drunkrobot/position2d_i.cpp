/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <orcaice/orcaice.h>
#include "position2d_i.h"

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
