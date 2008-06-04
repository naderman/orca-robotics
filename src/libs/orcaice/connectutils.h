/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_CONNECT_UTILITIES_H
#define ORCAICE_CONNECT_UTILITIES_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaice/context.h>
#include <orcaice/exceptions.h>
#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/configutils.h>
#include <hydroutil/sysutils.h>
#include <orcaice/printutils.h>
#include <orcaice/stringutils.h>

namespace orcaice
{
/*!
 *  @name Connection Functions
 */
//@{

/*!
 *  Tries to ping the default Locator and, based on success or the exceptions
 *  it catches, determines if the registry is reachable.
 */
bool isRegistryReachable( const Context& context );

/*!
 Tries to reach the remote interface specified with proxyString by calling
 ice_ping() function. Returns TRUE if the ping was successful and FALSE if not.
 Writes diagnostic information into @c diagnostic string.
 
 Catches all exceptions. Does not throw.  
  
 Implementation note: this function does not need to be templated because
 ice_ping() is implemented by all Ice objects regardless of type.
 */
bool isInterfaceReachable( const Context& context,
                           const std::string& proxyString,  
                           std::string& diagnostic );

/*!
 *  Adds the @p object to the component adapter and gives it the @p name.
 *  Note that @p name is just the interface name, not its fully-qualified name.
 *  (In Ice terms this will become the object identity.)
 *
 *  Throws gbxutilacfr::Exception if fails to register the new object.
 *
@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithString( context(), obj, "coolname" );
@endverbatim
 */
void createInterfaceWithString( const Context       & context,
                                Ice::ObjectPtr      & object,
                                const std::string   & name );
                            
/*!
 * Convenience function, behaves like @ref createInterfaceWithString but the proxy information
 * is looked up in the @p context properties based on the @p interfaceTag.
 *
 * Throws ConfigFileException if the interface name cannot be read from the config file for some reason.
 *
@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithTag( context(), obj, "InterfaceTag" );
@endverbatim
 */
void createInterfaceWithTag( const Context      & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & interfaceTag );

/*!
 * Adds an object to the adapter and returns a direct proxy to the object. A
 * globally unique name is supplied. This function is useful for creating
 * 'consumer' objects, i.e. you subscribe for information to be pushed into
 * them by specifying the direct proxy to them.
 *
 * This is a local operation which does not throw any exceptions.

@verbatim
Ice::ObjectPtr consumer = new Odometry2dConsumerI;
orca::Odometry2dConsumerPrx consumerPrx =
    orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context(), consumer );
@endverbatim
 */
template<class ConsumerProxyType>
ConsumerProxyType
createConsumerInterface( const Context  & context,
                         Ice::ObjectPtr & consumer )
{
    // create servant and tell adapter about it (let it make up a globally unique name)
    // NOTE: addWithUUID() does not throw exceptions.
    try
    {
        Ice::ObjectPrx obj = context.adapter()->addWithUUID( consumer );

        // make a direct proxy
        Ice::ObjectPrx prx = context.adapter()->createDirectProxy( obj->ice_getIdentity() );
        return ConsumerProxyType::uncheckedCast( prx );
    }
    catch( const Ice::ObjectAdapterDeactivatedException &e )
    {
        std::stringstream ss;
        ss << "orcaice::Component: Failed to create consumer interface because the adapter is destroyed : " << e;
        context.tracer().warning( ss.str() );
        throw orcaice::ComponentDeactivatingException( ERROR_INFO, ss.str() );
    }
}

//
//  Implementation Note:
//  the two 'connectToInterface' functions have to be templated, unfortunately,
//  to ensure we're connecting to the right type.
/*!
 *  Create an Ice proxy @p proxy for the remote server based on a stringified proxy @p proxyString.
 *  The proxy can be direct or indirect. For indirect proxies with platform name set to @e local, 
 *  hostname is used as the platform name.
 *
 *  Throws TypeMismatchException if fails to connect to the remote interface or if
 *  it is of the wrong type. Throws NetworkException if the interface is otherwise unreachable.
 *
 *  Example: to connect to interface of type MyInterface, use
@verbatim
MyInterfacePrx myInterfacePrx;
orcaice::connectToInterfaceWithString<MyInterfacePrx>( context(), myInterfacePrx, "iface@platform/component" );
@endverbatim
 */
template<class ProxyType>
void
connectToInterfaceWithString( const Context     & context,
                              ProxyType         & proxy,
                              const std::string & proxyString)
{

    // for indirect proxies only: if platform is set to 'local', replace it by hostname
    std::string resolvedProxyString = resolveLocalPlatform( context, proxyString );

    Ice::ObjectPrx base = context.communicator()->stringToProxy( resolvedProxyString );

    // check with the server that the one we found is of the right type
    // the check operation is remote and may fail (see sec.6.11.2)
    try {
        proxy = ProxyType::checkedCast( base );
        // got some answer, check that it's the right type
        if ( !proxy ) {
            std::string errString = "Required interface '" + resolvedProxyString + "' is of wrong type.";
            initTracerWarning( context, errString, 2 );
            throw orcaice::TypeMismatchException( ERROR_INFO, errString );
        }
    }
    // typically we get ConnectionRefusedException, but it may be ObjectNotExistException
    catch ( Ice::LocalException &e )
    {
        // Give some feedback as to why shit isn't working
        std::stringstream ss;
        ss << "Failed to connect to '" << resolvedProxyString << "': "<<e;
        initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }

    //! @todo Record who we connected to as a required interface so that Home can report this.
}

/*!
 *  Convenience function, behaves like @ref connectToInterfaceWithString but the proxy information
 *  comes from the configuration file and the @p interfaceTag.
 *
@verbatim
MyInterfacePrx myInterfacePrx;
orcaice::connectToInterfaceWithTag<MyInterfacePrx>( context(), myInterfacePrx, "MyInterface" );
@endverbatim
 *  Throws ConfigFileException if the interface name cannot be read for some reason.
 */
template<class ProxyType>
void
connectToInterfaceWithTag( const Context       & context,
                           ProxyType           & proxy,
                           const std::string   & interfaceTag )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    connectToInterfaceWithString( context, proxy, proxyString );
}

//! Tries to connect the interface described in the config file and gets its ID.
//! Throws NetworkException if the interface is unreachable.
std::string getInterfaceIdWithString( const Context& context, const std::string& proxyString );

//! Tries to connect the interface described in the config file and gets its ID.
//! Throws NetworkException if the interface is unreachable.
//! Throws ConfigFileException if there's a problem with reading from the config file.
std::string getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag );

// FUNCTIONS WITHOUT DOXYGEN TAGS ARE UTILITY FUNCTIONS
// THEY ARE PUBLICLY AVAILABLE BUT ARE NOT ADVERTIZED THROUGH DOXYGEN

/*
 * Behaves the same as the one above but connects to the topic manager
 * specified in the current properties.
 */
IceStorm::TopicPrx connectToIceStormTopicPrx( const Context &,
                                              const std::string & topicName,
                                              bool createIfMissing=false );

/*
 * Behaves the same as the one above but connects to the topic manager
 * specified in the communicator's properties.
 */
IceStorm::TopicPrx connectToIceStormTopicPrx( const Ice::CommunicatorPtr & communicator,
                                              const std::string & topicName,
                                              bool createIfMissing=false );

/*
 * Given the stingified proxy to topic manager and topic name, connect to the topic proxy.
 * If topic manager does not exist, throws Exception exception.
 */
IceStorm::TopicPrx connectToIceStormTopicPrxWithManager( const Ice::CommunicatorPtr & communicator,
                                    const std::string & topicName,
                                    const std::string & topicManagerString,
                                    bool createIfMissing=false );

/*
 * Publisher is used from the provider end. It is the consumer of information.
 * So you can push data into it.
 */
Ice::ObjectPrx connectToIceStormTopicPublisherPrx( const Ice::CommunicatorPtr & communicator,
                                    const std::string & topicName );

/*
 * Behaves like the one above.
 */
Ice::ObjectPrx connectToIceStormTopicPublisherPrx( const IceStorm::TopicPrx & topic );


/*!
 *  Convenience function, behaves like @ref connectToTopicWithString but the proxy information
 *  comes from the configuration file and the @p interfaceTag. 
 *  The appropriate topic name is generated based on the fully-qualified interface name. Default
 *  subtopic is "*".
 *
 *  Throws ConfigFileException if the interface name cannot be read for some reason.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithTag( const Context           & context,
                       ConsumerProxyType       & publisher,
                       const std::string       & interfaceTag,
                       const std::string       & subtopic="*" )
{
    context.tracer().debug( "orcaice::connectToTopicWithTag() tag="+interfaceTag, 10 );

    // lookup the name of the interface in the config file and generate topic name.
    // this generates a standard topic name based on fully-qualified interface name.
    std::string topicName = orcaice::toString(
                    orcaice::getProvidedTopicWithTag( context, interfaceTag, subtopic ) );

    // do the conversion to string by hand, to cut dependency on libOrcaObj
    // see <orcaobj/stringutils.cpp>
//     orca::FQTopicName name = orcaice::getProvidedTopicWithTag( context, interfaceTag, subtopic );
//     std::string topicName =  name.iface + "/" + name.topic + "@" + name.platform + "/" + name.component;

    return connectToTopicWithString( context, publisher, topicName );
}

/*!
 *  This function is used by potential publishers of information. It returns
 *  a proxy to the IceStorm topic and sets proxy to the topic's @p publisher.
 *  The publisher is used to distribute information to consumers and the
 *  topic is used to subscribe/unsubscribe data consumers.
 *
 *  The address of IceStorm server is looked up in the @c IceStorm.TopicManager.Proxy property.
 *
 *  Raises NetworkException if the server cannot be reached or Exception if there is any other
 *  problem.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithString( const Context     & context,
                          ConsumerProxyType & publisher,
                          const std::string & topicName )
{
    IceStorm::TopicPrx topicPrx;

    try {
        const bool createIfMissing = true;
        //
        // set the proxy to the topic
        //
        topicPrx = connectToIceStormTopicPrx( context.communicator(), topicName, createIfMissing );

        Ice::ObjectPrx obj = connectToIceStormTopicPublisherPrx( topicPrx );
        //
        // set the proxy to the publisher
        //
        publisher = ConsumerProxyType::uncheckedCast(obj);
    }
    //catch ( const gbxutilacfr::Exception & e ) {
        // we'll catch it here if the topic manager does not exist
    //}
    catch ( Ice::ConnectionRefusedException &e )
    {
        // Give some feedback as to why this isn't working
        std::stringstream ss; ss<<"Error while connecting to IceStorm topic publisher '"<<topicName<<"': "<<e;
        initTracerError( context, ss.str(), 2 );
        initTracerInfo( context, "hint: Is IceStorm running?", 10 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch( const Ice::LocalException &e )
    {
        std::stringstream ss;
        ss<<"Error while connecting to IceStorm topic publisher '"<<topicName<<"': "<<e;
        initTracerError( context, ss.str(), 2 );
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    catch ( Ice::Exception &e )
    {
        // Give some feedback as to why this isn't working
        std::stringstream ss; ss<<"Error while connecting to IceStorm topic publisher '"<<topicName<<"': "<<e;
        initTracerError( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }

    return topicPrx;
}

//@}

} // namespace

#endif
