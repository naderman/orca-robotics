/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCACM_COMPONENT_MODEL_LOOKUP_UTILITIES_H
#define ORCACM_COMPONENT_MODEL_LOOKUP_UTILITIES_H

#include <orcaice/context.h>
#include <orcacm/types.h>

namespace orcacm
{
/*!
 *  @name Lookup Functions
 */
//@{

/*!
 *  Attempts to ping the @ref orca_interface_home interface of the component
 *  whose stringified name is given as @p adapterId. Returns TRUE if successful.
 */
bool
pingComponent( const orcaice::Context & context, const std::string & adapterId );

/*!
 *  Connects to IceGrid/Admin interface of the specified registry and gets the
 *  list of registered adapters. If connection fails for some reason,
 *  @p isReachable field of ComponentData is set to FALSE.
 *
 *  When using the default registry, you can obtain the @p locatorString like this:
 *  communicator()->getDefaultLocator()->ice_toString()
 *
 *  If @p pingAdapters is TRUE (default), the function attempts to ping each
 *  registered adapter. Otherwise it doesn't and the @p isReachable field for
 *  each adapter is set to FALSE;
 */
RegistryData
getRegistryData( const orcaice::Context & context, const std::string & locatorString, bool pingAdapters=true );

RegistryHomeData
getRegistryHomeData( const orcaice::Context & context, const std::string & locatorString, bool pingAdapters=true );

/*!
 *  Connects to Home interface of the specified component and gets its
 *  deployment and interface information. If connection fails for some reason,
 *  @p isReachable field of ComponentData is set to FALSE.
 *
 *  If you already have an instance of RegistryData, do this:
 *  getComponentData( context, registryData.adapters[i].name )
 */
ComponentData
getComponentData( const orcaice::Context & context, const orca::FQComponentName & component );

ComponentData
getComponentHomeData( const orcaice::Context & context, const Ice::ObjectPrx & home );

/*!
 *  Convenience function, behaves like the one above.
 */
ComponentData
getComponentData( const orcaice::Context & context, const std::string & adapterId );

/*!
 *  Tries to connect to the specified interface and, if successful, gets its ID.
 *  If connection fails for some reason, the Object ID is set to "unknown".
 */
ProvidesHeader
getProvidesHeader( const orcaice::Context & context, const orca::FQInterfaceName & fqName );

/*!
 *  Tries to connect to the specified interface and, if successful, gets its ID.
 *  If connection fails for some reason, the Object ID is set to "unknown".
 */
RequiresHeader
getRequiresHeader( const orcaice::Context & context, const orca::FQInterfaceName & fqName );

//@}
} // namespace

#endif
