/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_MULTI_CONNECT_UTILITIES_H
#define ORCAICE_MULTI_CONNECT_UTILITIES_H

#include <orcaice/connectutils.h>
#include <orcaice/thread.h>

namespace orcaice
{
/*!
 *  @name Multi-Try Connection Functions
 */
//@{

/*!
Convenience function. Tries to setup the specified interface until is successful or
the @c thread is stopped. 

We catch orcaice::Exception, sleep for @c retryInterval [s] and try again.

Example:
@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithString( context_, obj, "coolname", (orcaice::Thread*)this );
@endverbatim
 */
void createInterfaceWithString( const Context       & context,
                                Ice::ObjectPtr      & object,
                                const std::string   & name,
                                orcaice::Thread*  thread, int retryInterval=2 );

/*!
Convenience function. Tries to setup the specified interface until is successful or
the @c thread is stopped.

We catch orcaice::Exception, sleep for @c retryInterval [s] and try again.

We do NOT catch a possible orcaice::ConfigFileException exception.

@verbatim
Ice::ObjectPtr obj = new MyObjectI;
try
{
    orcaice::createInterfaceWithTag( context_, obj, "InterfaceTag", (orcaice::Thread*)this );
}
catch ( const orcaice::ConfigFileException& e ) {
    // what do we do?
}
@endverbatim
 */
void createInterfaceWithTag( const Context      & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & interfaceTag,
                            orcaice::Thread*  thread, int retryInterval=2 );

/*!
    Tries to activate the adapter (by calling Context::activate(). If fails, sleeps for
    @c retryInterval [s] seconds. Will repeat indefinitely until the thread is stopped (checks
    orcaice::Thread::isAlive() ).
*/
void activate( Context& context, orcaice::Thread* thread, int retryInterval=2 );

/*!
Convenience function. Tries to connect to the specified remote interface until is successful or
the @c thread is stopped. 

We catch orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

We do NOT catch a possible orcaice::TypeMismatchException because this condition is unlikely to
change.

Example:
@verbatim
MyInterfacePrx myInterfacePrx;
try {
    orcaice::connectToInterfaceWithString<MyInterfacePrx>( 
        context_, myInterfacePrx, "iface@platform/component", (orcaice::Thread*)this );
}
catch ( const orcaice::TypeMismatchException& e ) {
    // what do we do?
}
@endverbatim
 */
template<class ProxyType>
void
connectToInterfaceWithString( const Context     & context,
                              ProxyType         & proxy,
                              const std::string & proxyString,
                              orcaice::Thread*    thread, int retryInterval=2 )
{    
    context.tracer()->debug( "orcaice::connectToInterfaceWithString(thread) proxy="+proxyString, 10 );
    while ( thread->isActive() )
    {
        try {
            connectToInterfaceWithString<ProxyType>( context, proxy, proxyString );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to interface with string "<<proxyString<<". "
                <<"Will retry in "<<retryInterval<<"s."
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

/*!
Convenience function. Tries to connect to the specified remote interface until is successful or
the @c thread is stopped. 

We catch orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

We do NOT catch a possible orcaice::TypeMismatchException because this condition is unlikely to
change.

We do NOT catch a possible orcaice::ConfigFileException exception.

Example:
@verbatim
MyInterfacePrx myInterfacePrx;
try {
    orcaice::connectToInterfaceWithTag<MyInterfacePrx>( 
        context_, myInterfacePrx, "MyInterface", (orcaice::Thread*)this );
}
catch ( const orcaice::TypeMismatchException& e ) {
    // what do we do?
}
catch ( const orcaice::ConfigFileException& e ) {
    // what do we do?
}
@endverbatim
 */
template<class ProxyType>
void
connectToInterfaceWithTag( const Context     & context,
                           ProxyType         & proxy,
                           const std::string & interfaceTag,
                           orcaice::Thread*  thread, int retryInterval=2 )
{    
    context.tracer()->debug( "orcaice::connectToInterfaceWithTag(thread) tag="+interfaceTag, 10 );
    while ( thread->isActive() )
    {
        try {
            connectToInterfaceWithTag<ProxyType>( context, proxy, interfaceTag );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to interface with tag "<<interfaceTag<<". "
                <<"Will retry in "<<retryInterval<<"s."
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

/*!
Convenience function. Tries to connect to the specified topic by calling connectToTopicWithString() 
until is successful or the @c thread is stopped (in this case an empty topic proxy is returned).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithString( const Context     & context,
                          ConsumerProxyType & publisher,
                          const std::string & topicName,
                          orcaice::Thread*  thread, int retryInterval=2 )
{
    context.tracer()->debug( "orcaice::connectToTopicWithString(thread) topic="+topicName, 10 );
    IceStorm::TopicPrx topicPrx;

    while ( thread->isActive() )
    {
        try {
            topicPrx = connectToTopicWithString<ConsumerProxyType>( context, publisher, topicName );
            break;
        }
        catch ( const orcaice::NetworkException& e )  {
            std::stringstream ss;
            ss << "Failed to connect to topic with string "<<topicName<<". "
                <<"Will retry in "<<retryInterval<<"s."
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
    return topicPrx;
}

/*!
Convenience function. Tries to connect to the specified topic by calling connectToTopicWithTag() 
until is successful or the @c thread is stopped (in this case an empty topic proxy is returned).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithTag( const Context      & context,
                       ConsumerProxyType  & publisher,
                       const std::string  & interfaceTag,
                       const std::string  & subtopic,
                       orcaice::Thread*  thread, int retryInterval=2 )
{
    context.tracer()->debug( "orcaice::connectToTopicWithTag(thread) tag="+interfaceTag, 10 );
    IceStorm::TopicPrx topicPrx;

    while ( thread->isActive() )
    {
        try {
            topicPrx = connectToTopicWithTag<ConsumerProxyType>( context, publisher, interfaceTag, subtopic );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to topic with tag "<<interfaceTag<<". "
                <<"Will retry in "<<retryInterval<<"s."
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
    return topicPrx;
}

/*!
Convenience function. Tries to connect to the specified topic by calling getInterfaceIdWithString() 
until is successful or the @c thread is stopped (in this case an empty string is returned).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.
*/
std::string getInterfaceIdWithString( const Context& context, const std::string& proxyString,
                       orcaice::Thread*  thread, int retryInterval=2 );

/*!
Convenience function. Tries to connect to the specified topic by calling getInterfaceIdWithString() 
until is successful or the @c thread is stopped (in this case an empty string is returned).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
*/
std::string getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag,
                       orcaice::Thread*  thread, int retryInterval=2 );

//@}

} // namespace

#endif
