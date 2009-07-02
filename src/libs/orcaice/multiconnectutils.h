/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_MULTI_CONNECT_UTILITIES_H
#define ORCAICE_MULTI_CONNECT_UTILITIES_H

#include <orcaice/connectutils.h>
// we only need the definition of Stoppable and checkedSleep() function.
// (we don't need the actual Thread class).
#include <gbxsickacfr/gbxiceutilacfr/threadutils.h>

namespace orcaice
{
/*!
 *  @name Multi-Try Connection Functions
 */
//@{

/*!
Convenience function. Tries to connect to the specified remote interface until successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface.

Nothing is done if retryNumber=0.  If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch orcaice::NetworkException, sleep for @c retryIntervalSec [s] and try again.

We do NOT catch a possible orcaice::TypeMismatchException because this condition is unlikely to
change.

Example:
@verbatim
MyInterfacePrx myInterfacePrx;
try {
    orcaice::connectToInterfaceWithString<MyInterfacePrx>( 
        context_, myInterfacePrx, "iface@platform/component", (gbxutilacfr::Stoppable*)this );
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
                              gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                              int retryIntervalSec=2, int retryNumber=-1 )
{        
    assert( activity && "Null activity pointer" );

    context.tracer().debug( subsysName, "orcaice::connectToInterfaceWithString(activity) proxy="+proxyString, 10 );
    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            connectToInterfaceWithString( context, proxy, proxyString );
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to interface with string "<<proxyString<<" : "<< e.what()
               << "\nWill retry in "<<retryIntervalSec<<"s.";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
    }
}

/*!
Convenience function. Tries to connect to the specified remote interface until successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface.
 
Nothing is done if retryNumber=0.  If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch orcaice::NetworkException, sleep for @c retryIntervalSec [s] and try again.

We do NOT catch a possible orcaice::TypeMismatchException because this condition is unlikely to
change.

We do NOT catch a possible orcaice::ConfigFileException exception.

Example:
@verbatim
MyInterfacePrx myInterfacePrx;
try {
    orcaice::connectToInterfaceWithTag<MyInterfacePrx>( 
        context_, myInterfacePrx, "MyInterface", (gbxutilacfr::Stoppable*)this );
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
                           gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                           int retryIntervalSec=2, int retryNumber=-1 )
{    
    assert( activity && "Null activity pointer" );

    context.tracer().debug( subsysName, "orcaice::connectToInterfaceWithTag(activity) tag="+interfaceTag, 10 );

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            connectToInterfaceWithTag<ProxyType>( context, proxy, interfaceTag );
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to interface with tag "<<interfaceTag<<": "<<e.what()
               << "\nWill retry in "<<retryIntervalSec<<"s.";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
    }
}

/*!
Convenience function. Tries to connect to the specified interface by calling getInterfaceIdWithString() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface. 

If unsuccesful for any reason, an empty string is returned. Nothing is done if retryNumber=0.  
If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryIntervalSec [s] and try again.
*/
std::string getInterfaceIdWithString( const Context& context, const std::string& proxyString,
                            gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                            int retryIntervalSec=2, int retryNumber=-1 );

/*!
Convenience function. Tries to connect to the specified interface by calling getInterfaceIdWithString() 
until successful, the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface. 

If unsuccesful for any reason, an empty string is returned. Nothing is done if retryNumber=0.  
If a non-empty subsystem name is supplied, 
sends a Status heartbeat after every attempt (@see gbxutilacfr::Status).

We catch all orcaice::NetworkException, sleep for @c retryIntervalSec [s] and try again.

All other exceptions are not likely to be resolved over time so we don't catch them.
*/
std::string getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag,
                            gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                            int retryIntervalSec=2, int retryNumber=-1 );

template<class InterfacePrxType, typename DescriptionType>
void
getDescriptionWithTag( const Context          &context,
                       const std::string      &interfaceTag,
                       DescriptionType        &description,
                       gbxutilacfr::Stoppable *activity,
                       const std::string      &subsysName       = "", 
                       int                     retryIntervalSec = 2, 
                       int                     retryNumber      = -1 )
{
    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            description = orcaice::getDescriptionWithTag<InterfacePrxType,DescriptionType>( context, interfaceTag );
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const orcaice::NetworkException& e ) {
            std::stringstream ss;
            ss << "Failed to connect to interface with tag "<<interfaceTag<<" : "<< e.what()
               << "\nWill retry in "<<retryIntervalSec<<"s.";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
    }
}


//@}

} // namespace

#endif
