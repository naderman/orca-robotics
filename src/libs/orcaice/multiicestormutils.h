/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_MULTI_ICESTORM_UTILITIES_H
#define ORCAICE_MULTI_ICESTORM_UTILITIES_H

#include <orcaice/icestormutils.h>
// we only need the definition of Stoppable and checkedSleep() function.
// (we don't need the actual Thread class).
#include <gbxsickacfr/gbxiceutilacfr/threadutils.h>

namespace orcaice
{
/*!
 *  @name Multi-Try IceStorm-Related Functions
 */
//@{

/*!
Convenience function. Tries to connect to the specified topic by calling connectToTopicWithString() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface.
 
If unsuccesful for any reason, an empty topic proxy is returned. Nothing is done if retryNumber=0.  
If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryIntervalSec [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithString( const Context     & context,
                          ConsumerProxyType & publisher,
                          const std::string & topicName,
                          gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                          int retryIntervalSec=2, int retryNumber=-1,
                          bool localReportingOnly=false )
{
    assert( activity && "Null activity pointer" );

    IceStorm::TopicPrx topicPrx;

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            topicPrx = connectToTopicWithString<ConsumerProxyType>( context, publisher, topicName );
            break;
        }
        catch ( const orcaice::NetworkException& e )  {
            std::stringstream ss;
            ss << "Failed to connect to topic with string "<<topicName<<". "
                <<"Will retry in "<<retryIntervalSec<<"s."
                << e.what();
            // not issuing status warnings because we would need to add localReportingOnly option
            context.tracer().warning( subsysName, ss.str(), 1, localReportingOnly );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return topicPrx;
}

/*!
Convenience function. Tries to connect to the specified topic by calling connectToTopicWithTag() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface. 

If unsuccesful for any reason, an empty topic proxy is returned.
Nothing is done if retryNumber=0.  

If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryIntervalSec [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
 */
template<class ConsumerProxyType>
IceStorm::TopicPrx
connectToTopicWithTag( const Context      & context,
                       ConsumerProxyType  & publisher,
                       const std::string  & interfaceTag,
                       const std::string  & subtopic,
                       gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                       int retryIntervalSec=2, int retryNumber=-1,
                       bool localReportingOnly=false )
{
    assert( activity && "Null activity pointer" );

    IceStorm::TopicPrx topicPrx;

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            topicPrx = connectToTopicWithTag<ConsumerProxyType>( context, publisher, interfaceTag, subtopic );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to topic with tag "<<interfaceTag<<". "
                <<"Will retry in "<<retryIntervalSec<<"s."
                << e.what();
            // not issuing status warnings because we would need to add localReportingOnly option
            context.tracer().warning( subsysName, ss.str(), 1, localReportingOnly );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return topicPrx;
}

//@}

} // namespace

#endif
