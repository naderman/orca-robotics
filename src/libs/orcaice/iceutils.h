/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICE_UTILITIES_H
#define ORCAICE_ICE_UTILITIES_H

#include <Ice/Object.h>
#include <Ice/Identity.h>

#include <orcaice/context.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>

namespace orcaice
{
/*!
 *  @name Ice-Related Functions
 */
//@{

/*!
Adds the @p object to the component adapter and gives it the @p name.
Note that @p name is just the interface name, not its fully-qualified name.
(In Ice terms this will become the object identity.)

Throws gbxutilacfr::Exception if fails to register the new object.

@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithString( context(), obj, "coolname" );
@endverbatim
 */
void createInterfaceWithString( const Context       & context,
                                Ice::ObjectPtr      & object,
                                const std::string   & name );
                            
/*!
Convenience function, behaves like @ref createInterfaceWithString but the proxy information
is looked up in the @p context properties based on the @p interfaceTag.

Throws ConfigFileException if the interface name cannot be read from the config file for some reason.

@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createInterfaceWithTag( context(), obj, "InterfaceTag" );
@endverbatim
 */
void createInterfaceWithTag( const Context      & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & interfaceTag );

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

//! Remove the interface from the adapter if possible.
//! If something goes wrong, fails silently.
void tryRemoveInterface( orcaice::Context& context, const std::string& interfaceName );

void tryRemoveInterfaceWithIdentity( orcaice::Context& context, const Ice::Identity& interfaceId );


//@}

} // namespace

#endif
