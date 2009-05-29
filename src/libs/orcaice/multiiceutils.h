/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_MULTI_ICE_UTILITIES_H
#define ORCAICE_MULTI_ICE_UTILITIES_H

#include <Ice/Object.h>

#include <orcaice/context.h>
#include <gbxutilacfr/stoppable.h>

namespace orcaice
{
/*!
 *  @name Multi-Try Ice-Related Functions
 */
//@{

/*!
Convenience function. Tries to setup the specified interface until is successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface.

Nothing is done if retryNumber=0.

Status warnings are set between retries if subsystem name is specified (@see gbxutilacfr::Status), 
otherwise warnings are simply traced.

We catch gbxutilacfr::Exception, sleep for @c retryIntervalSec [s] and try again.

Example:
@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithString( context_, obj, "coolname", (gbxutilacfr::Stoppable*)this );
@endverbatim
 */
void createInterfaceWithString( const Context       & context,
                                Ice::ObjectPtr      & object,
                                const std::string   & name,
                                gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                                int retryIntervalSec=2, int retryNumber=-1 );

/*!
Convenience function. Tries to setup the specified interface until successful,
the number of retries is exceeded (default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface.

Nothing is done if retryNumber=0.  

Status warnings are set between retries if subsystem name is specified (@see gbxutilacfr::Status), 
otherwise warnings are simply traced.

We catch gbxutilacfr::Exception, sleep for @c retryIntervalSec [s] and try again.
Gives up after @c retryNumber of attempts (-1 stands for infinite number of retries).

We do NOT catch a possible orcaice::ConfigFileException exception.

@verbatim
Ice::ObjectPtr obj = new MyObjectI;
try
{
    orcaice::createInterfaceWithTag( context_, obj, "InterfaceTag", (gbxutilacfr::Stoppable*)this );
}
catch ( const orcaice::ConfigFileException& e ) {
    // what do we do?
}
@endverbatim
 */
void createInterfaceWithTag( const Context      & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & interfaceTag,
                            gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                            int retryIntervalSec=2, int retryNumber=-1 );

/*!
Tries to activate the adapter (by calling Context::activate(). If fails, sleeps for
@c retryIntervalSec [s] seconds. Will repeat until successful, the number of retries is exceeded 
(default -1, i.e. infinite), or the @c activity is stopped.
Threads are a commonly used activity which implement Stoppable interface. 

Nothing is done if retryNumber=0.

If a non-empty subsystem name is supplied, sends a Status heartbeat after every 
attempt (@see gbxutilacfr::Status). Status warnings are NOT issued because the Status
interface is not accessible until the adapter is activated.

If a non-empty subsystem name is supplied, sends a Status heartbeat at every iteration 
(@see gbxutilacfr::Status).
*/
// NOTE: Context::activate() is not a const function, that's why a ref to it is not const.
void activate( Context& context, 
                gbxutilacfr::Stoppable* activity, const std::string& subsysName="", 
                int retryIntervalSec=2, int retryNumber=-1 );

//@}

} // namespace

#endif
