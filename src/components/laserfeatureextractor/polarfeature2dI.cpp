/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "polarfeature2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;
using namespace laserfeatures;

PolarFeature2dI::PolarFeature2dI( PtrBuffer<PolarFeature2dDataPtr> &polarFeatureDataBuffer,
                                  const IceStorm::TopicPrx &topicPrx ) 
    : polarFeatureDataBuffer_( polarFeatureDataBuffer ),
      topicPrx_(topicPrx)
{
}

// served out the data to the client (it was stored here by the driver at the last read)
PolarFeature2dDataPtr PolarFeature2dI::getData(const Ice::Current& current) const
{
    std::cout << "Sending data back" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( polarFeatureDataBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    // create a null pointer. data will be cloned into it.
    orca::PolarFeature2dDataPtr data;
    polarFeatureDataBuffer_.get( data );

    return data;
}

void PolarFeature2dI::subscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"INFO(polarfeature2d_i.cpp): Subscribtion request"<<endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

void PolarFeature2dI::unsubscribe(const ::orca::PolarFeature2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"INFO(polarfeature2d_i.cpp): Unsubscribe request"<<endl;
    topicPrx_->unsubscribe( subscriber );
}
