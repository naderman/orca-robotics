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

//! Remove the interface from the adapter if possible.
//! If something goes wrong, fails silently.
void tryRemoveInterface( orcaice::Context& context, const std::string& interfaceName );

void tryRemoveInterfaceWithIdentity( orcaice::Context& context, const Ice::Identity& interfaceId );

//@}

} // namespace

#endif
