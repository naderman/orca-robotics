/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "connectutils.h"
#include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

void
createInterfaceWithString( const Context      & context,
                            Ice::ObjectPtr    & object,
                            const std::string & proxyString )
{
    try
    {
        // register object with the adapter
        context.adapter()->add( object, context.communicator()->stringToIdentity(proxyString) );
    }
    catch ( const Ice::AlreadyRegisteredException &e )
    {
        std::stringstream ss;
        ss<<"Failed to create interface "<<proxyString<<": "<<e;
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }

    // locally register this interface with Home interface 
    orca::ProvidedInterface iface;
    iface.name = proxyString;
    // is this a local call? is there a better way?
    iface.id   = object->ice_id();
    context.home()->addProvidedInterface( iface );
}

void
createInterfaceWithTag( const Context      & context,
                        Ice::ObjectPtr     & object,
                        const std::string  & ifaceTag )
{
    // look up naming information in the properties
    orca::FQInterfaceName fqIName = getProvidedInterface( context, ifaceTag );

    createInterfaceWithString( context, object, fqIName.iface );
}

std::string 
getInterfaceIdWithString( const Context& context, const std::string& proxyString )
{
    Ice::ObjectPrx prx = context.communicator()->stringToProxy(proxyString);

    // check with the server that the one we found is of the right type
    // the check operation is remote and may fail (see sec.6.11.2)
    try {
        return prx->ice_id();
    }
    // typically we get ConnectionRefusedException, but it may be ObjectNotExistException
    catch ( Ice::LocalException &e )
    {
        // Give some feedback as to why shit isn't working
        std::stringstream ss;
        ss << "Failed to lookup ID of '" << proxyString << "': "<<e;
        initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
}

std::string 
getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    return getInterfaceIdWithString( context, proxyString );
}

IceStorm::TopicPrx 
connectToIceStormTopicPrx( const Context & context,
                                        const std::string   & topicName,
                                        bool                createIfMissing )
{
    return connectToIceStormTopicPrx( context.communicator(), topicName, createIfMissing );
}


IceStorm::TopicPrx
connectToIceStormTopicPrx( const Ice::CommunicatorPtr & communicator,
                           const std::string          & topicName,
                           bool                       createIfMissing )
{
    // find IceStorm, there's a default value for this prop set in setDefaultOrcaProperties()
    std::string topicManagerString =
            communicator->getProperties()->getProperty("IceStorm.TopicManager.Proxy");
    
    return connectToIceStormTopicPrxWithManager( communicator, topicName, topicManagerString, createIfMissing );

}

// tracer already created
IceStorm::TopicPrx
connectToIceStormTopicPrxWithManager( const Ice::CommunicatorPtr & communicator,
                                      const std::string          & topicName,
                                      const std::string          & topicManagerString,
                                      bool                       createIfMissing )
{
    Ice::ObjectPrx base = communicator->stringToProxy( topicManagerString );
    IceStorm::TopicManagerPrx stormManager = IceStorm::TopicManagerPrx::checkedCast(base);
    if ( !stormManager ) 
    {
        std::string errorString = "invalid proxy to IceStorm";
        communicator->getLogger()->error( errorString );
        throw( hydroutil::Exception( ERROR_INFO, errorString ) );
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
            //throw hydroutil::Exception(ERROR_INFO,"Couldn't create topic");
        }
    }
    
    // debug
/*    
    sleep(1);
    try {
        topic->ice_ping();
    }
    catch( const Ice::Exception & e)
    {
        std::string errorString = "failed to ping topic "+topicName+" on server "+topicManagerString;
        communicator->getLogger()->error( errorString );
        std::cout<<e<<std::endl;
        exit(0);
    }
*/
    return topic;
}

// this version of the function is called before the standard tracer is created,
// so we use tracing functions.
Ice::ObjectPrx 
connectToIceStormTopicPublisherPrx( const Ice::CommunicatorPtr & communicator,
                                                   const std::string          & topicName )
{
    // Retrieve the topic by name, if does not exist: create
    IceStorm::TopicPrx topic = connectToIceStormTopicPrx( communicator, topicName, true );

    // now that we have the topic itself, get its publisher object
    return connectToIceStormTopicPublisherPrx( topic );
}

Ice::ObjectPrx 
connectToIceStormTopicPublisherPrx( const IceStorm::TopicPrx & topic )
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

bool
isRegistryReachable( const Context& context )
{
    assert( context.communicator() );

    try 
    {
        Ice::LocatorPrx locator = context.communicator()->getDefaultLocator();

        // debug
        context.tracer()->debug( "pinging "+context.communicator()->proxyToString( locator ),5 );
        
        // ping the registry
        locator->ice_ping();

        return true;
    }
    catch( const Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "orcaice::isRegistryReachable(): caught exception: " << e;
        context.tracer()->debug( ss.str() ,5 );
    }
    catch( const std::exception &e )
    {
        std::stringstream ss;
        ss << "orcaice::isRegistryReachable(): caught exception: " << e.what();
        context.tracer()->debug( ss.str() ,5 );
    }
    return false;
}

bool 
isInterfaceReachable( const Context& context, const std::string& proxyString,  
                           std::string& diagnostic )
{
    try {
        Ice::ObjectPrx obj = context.communicator()->stringToProxy( proxyString );
        obj->ice_ping();
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss; 
        ss << e;
        diagnostic = ss.str();
        return false;
    }

    return true;
}

} // namespace
