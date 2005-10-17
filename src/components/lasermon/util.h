#ifndef LASERMON_UTIL_H
#define LASERMON_UTIL_H

#include <string>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

namespace lasermonutil {

template<class ConsumerType>
void
subscribeConsumer( const Ice::CommunicatorPtr &communicator,
                   const Ice::ObjectAdapterPtr adapter,
                   ConsumerType &consumer, 
                   const std::string topicTag )
{
    Ice::ObjectPrx prx = adapter->addWithUUID( consumer );

    // make a direct proxy
    Ice::ObjectPrx callbackPrx = adapter->createDirectProxy( prx->ice_getIdentity() );

    // Get the IceStorm topic
    std::string topicName    = orcaiceutil::getRemoteTopicName( communicator, topicTag );

    IceStorm::TopicPrx topicPrx = orcaiceutil::getIceStormTopic( communicator, topicName );
    assert(topicPrx);

    IceStorm::QoS qos;
    topicPrx->subscribe( qos, callbackPrx );
}


}

#endif
