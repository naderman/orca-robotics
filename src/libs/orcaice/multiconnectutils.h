/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_MULTI_CONNECT_UTILITIES_H
#define ORCAICE_MULTI_CONNECT_UTILITIES_H

#include <orcaice/connectutils.h>
#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <IceUtil/Thread.h>

namespace orcaice
{
/*!
 *  @name Multi-Try Connection Functions
 */
//@{

/*!
Convenience function. Tries to setup the specified interface until is successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c thread is stopped.
Nothing is done if retryNumber=0. If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch gbxutilacfr::Exception, sleep for @c retryInterval [s] and try again.

Example:
@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithString( context_, obj, "coolname", (gbxiceutilacfr::Thread*)this );
@endverbatim
 */
void createInterfaceWithString( const Context       & context,
                                Ice::ObjectPtr      & object,
                                const std::string   & name,
                                gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                                int retryInterval=2, int retryNumber=-1 );

/*!
Convenience function. Tries to setup the specified interface until successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c thread is stopped.
Nothing is done if retryNumber=0.  If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch gbxutilacfr::Exception, sleep for @c retryInterval [s] and try again.
Gives up after @c retryNumber of attempts (-1 stands for infinite number of retries).

We do NOT catch a possible orcaice::ConfigFileException exception.

@verbatim
Ice::ObjectPtr obj = new MyObjectI;
try
{
    orcaice::createInterfaceWithTag( context_, obj, "InterfaceTag", (gbxiceutilacfr::Thread*)this );
}
catch ( const orcaice::ConfigFileException& e ) {
    // what do we do?
}
@endverbatim
 */
void createInterfaceWithTag( const Context      & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & interfaceTag,
                            gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                            int retryInterval=2, int retryNumber=-1 );

/*!
Tries to activate the adapter (by calling Context::activate(). If fails, sleeps for
@c retryInterval [s] seconds. Will repeat until successful, the number of retries is exceeded 
(default -1, i.e. infinite), or the @c thread is stopped. Nothing is done if retryNumber=0.
If a non-empty subsystem name is supplied, sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

If a non-empty subsystem name is supplied, sends a Status heartbeat at every iteration 
(@see gbxutilacfr::Status).
*/
// note: Context::activate() is not a const function, that's why a ref to it is not const.
void activate( Context& context, 
                gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                int retryInterval=2, int retryNumber=-1 );

/*!
Convenience function. Tries to connect to the specified remote interface until successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c thread is stopped.
Nothing is done if retryNumber=0.  If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

We do NOT catch a possible orcaice::TypeMismatchException because this condition is unlikely to
change.

Example:
@verbatim
MyInterfacePrx myInterfacePrx;
try {
    orcaice::connectToInterfaceWithString<MyInterfacePrx>( 
        context_, myInterfacePrx, "iface@platform/component", (gbxiceutilacfr::Thread*)this );
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
                              gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                              int retryInterval=2, int retryNumber=-1 )
{    
    context.tracer().debug( "orcaice::connectToInterfaceWithString(thread) proxy="+proxyString, 10 );
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            connectToInterfaceWithString( context, proxy, proxyString );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to interface with string "<<proxyString<<". "
                <<"Will retry in "<<retryInterval<<"s."
                << e.what();
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
}

/*!
Convenience function. Tries to connect to the specified remote interface until successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c thread is stopped. 
Nothing is done if retryNumber=0.  If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

We do NOT catch a possible orcaice::TypeMismatchException because this condition is unlikely to
change.

We do NOT catch a possible orcaice::ConfigFileException exception.

Example:
@verbatim
MyInterfacePrx myInterfacePrx;
try {
    orcaice::connectToInterfaceWithTag<MyInterfacePrx>( 
        context_, myInterfacePrx, "MyInterface", (gbxiceutilacfr::Thread*)this );
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
                           gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                           int retryInterval=2, int retryNumber=-1 )
{    
    context.tracer().debug( "orcaice::connectToInterfaceWithTag(thread) tag="+interfaceTag, 10 );

    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
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
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
}

/*!
Convenience function. Tries to connect to the specified topic by calling connectToTopicWithString() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c thread is stopped 
If unsuccesful for any reason, an empty topic proxy is returned. Nothing is done if retryNumber=0.  
If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithString( const Context     & context,
                          ConsumerProxyType & publisher,
                          const std::string & topicName,
                          gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 )
{
    context.tracer().debug( "orcaice::connectToTopicWithString(thread) topic="+topicName, 10 );
    IceStorm::TopicPrx topicPrx;

    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
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
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return topicPrx;
}

/*!
Convenience function. Tries to connect to the specified topic by calling connectToTopicWithTag() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c thread 
is stopped. If unsuccesful for any reason, an empty topic proxy is returned.
Nothing is done if retryNumber=0.  If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithTag( const Context      & context,
                       ConsumerProxyType  & publisher,
                       const std::string  & interfaceTag,
                       const std::string  & subtopic,
                       gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                       int retryInterval=2, int retryNumber=-1 )
{
    context.tracer().debug( "orcaice::connectToTopicWithTag(thread) tag="+interfaceTag, 10 );
    IceStorm::TopicPrx topicPrx;

    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
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
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return topicPrx;
}

/*!
Convenience function. Tries to connect to the specified topic by calling getInterfaceIdWithString() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c thread 
is stopped. If unsuccesful for any reason, an empty string is returned. Nothing is done if retryNumber=0.  
If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.
*/
std::string getInterfaceIdWithString( const Context& context, const std::string& proxyString,
                            gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                            int retryInterval=2, int retryNumber=-1 );

/*!
Convenience function. Tries to connect to the specified topic by calling getInterfaceIdWithString() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c thread 
is stopped. If unsuccesful for any reason, an empty string is returned. Nothing is done if retryNumber=0.  
If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryInterval [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
*/
std::string getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag,
                            gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                            int retryInterval=2, int retryNumber=-1 );

//@}

} // namespace

#endif
