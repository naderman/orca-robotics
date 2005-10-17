#ifndef LASERMON_UTIL_H
#define LASERMON_UTIL_H

#include <string>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

namespace lasermonutil {

//! Connect to an IceStorm topic, based on a tag in the config file
//template<class ConsumerType>
void
subscribeConsumerToTopic( const Ice::CommunicatorPtr &communicator,
                          const Ice::ObjectAdapterPtr adapter,
//                       ConsumerType &consumer, 
                          Ice::ObjectPtr &consumer, 
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


//! create a proxy for the remote server based on a tag in the config file
template<class ProxyType>
void
connectProxy( const Ice::CommunicatorPtr &communicator,
              const Ice::ObjectAdapterPtr adapter,
              ProxyType &proxy, 
              const std::string topicTag )
{
    std::string proxyName = orcaiceutil::getRemotePortName( communicator, topicTag );

    Ice::ObjectPrx base = communicator->stringToProxy(proxyName);
    // check with the server that the one we found is of the right type

    int count = 0;
    while ( true ) 
    {
        try 
        {
            proxy = ProxyType::checkedCast(base);
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e ) 
        {
            if ( count++ > 5 ) exit(0);
            std::cout<<"tried "<<count<<" times"<<std::endl;
            sleep(1);
        }
    }
}


}

#endif
