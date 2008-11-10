/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "icestormutils.h"
#include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

IceStorm::TopicPrx 
connectToIceStormTopicPrx( const Context& context,
                            const std::string  & topicName,
                            bool                createIfMissing )
{
    return connectToIceStormTopicPrx( context.communicator(), topicName, createIfMissing );
}


IceStorm::TopicPrx
connectToIceStormTopicPrx( const Ice::CommunicatorPtr& communicator,
                           const std::string         & topicName,
                           bool                       createIfMissing )
{
    // find IceStorm, there's a default value for this prop set in setDefaultOrcaProperties()
    std::string topicManagerString =
            communicator->getProperties()->getProperty("IceStorm.TopicManager.Proxy");
    
    return connectToIceStormTopicPrxWithManager( communicator, topicName, topicManagerString, createIfMissing );

}

// tracer already created
IceStorm::TopicPrx
connectToIceStormTopicPrxWithManager( const Ice::CommunicatorPtr& communicator,
                                      const std::string         & topicName,
                                      const std::string         & topicManagerString,
                                      bool                       createIfMissing )
{
    Ice::ObjectPrx base = communicator->stringToProxy( topicManagerString );
    IceStorm::TopicManagerPrx stormManager = IceStorm::TopicManagerPrx::checkedCast(base);
    if ( !stormManager ) 
    {
        std::string errorString = "invalid proxy to IceStorm";
        communicator->getLogger()->error( errorString );
        throw( gbxutilacfr::Exception( ERROR_INFO, errorString ) );
        return IceStorm::TopicPrx();
    }
    
    // Retrieve the topic by name, if does not exist, return empty one, do not create
    IceStorm::TopicPrx topic;
    try
    {
        topic = stormManager->retrieve( topicName );
    }
    catch( const IceStorm::NoSuchTopic& e )
    {
        if ( createIfMissing ) {
            communicator->getLogger()->warning( "no such topic, name=" + e.name + ". Creating a new one." );
            // if the topic doesn't exist, create a new one.
            topic = stormManager->create( topicName );
        }
        else {
            communicator->getLogger()->warning( "no such topic, name=" + e.name );
            // do not create a new topic
            // @todo what do we do? rethrow?
            //throw gbxutilacfr::Exception(ERROR_INFO,"Couldn't create topic");
        }
    }
    return topic;
}

// this version of the function is called before the standard tracer is created,
// so we use tracing functions.
Ice::ObjectPrx 
connectToIceStormTopicPublisherPrx( const Ice::CommunicatorPtr& communicator, const std::string& topicName )
{
    // Retrieve the topic by name, if does not exist: create
    IceStorm::TopicPrx topic = connectToIceStormTopicPrx( communicator, topicName, true );

    // now that we have the topic itself, get its publisher object
    return connectToIceStormTopicPublisherPrx( topic );
}

Ice::ObjectPrx 
connectToIceStormTopicPublisherPrx( const IceStorm::TopicPrx& topic )
{
    // Get the topic's publisher object
    // @todo should we verify that the publisher is an object of the right type? need template.
    Ice::ObjectPrx obj = topic->getPublisher();
    
    // create a oneway proxy (for efficiency reasons).
    if(!obj->ice_isDatagram()) {
        obj = obj->ice_oneway();
    }

    return obj;
}

} // namespace
