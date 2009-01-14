/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_MULTI_ICESTORM_UTILITIES_H
#define ORCAICE_MULTI_ICESTORM_UTILITIES_H

#include <orcaice/icestormutils.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>

namespace orcaice
{
/*!
 *  @name Multi-Try IceStorm-Related Functions
 */
//@{

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
            bool localOnly = true;
            context.tracer().warning( ss.str(), 1, localOnly );
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
            bool localOnly = true;
            context.tracer().warning( ss.str(), 1, localOnly );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return topicPrx;
}

//@}

} // namespace

#endif
