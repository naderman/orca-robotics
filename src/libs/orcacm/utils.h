/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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
 *  Attempts to ping the well-known object with specified identity. Returns TRUE if successful.
 */
bool
pingObject( const orcaice::Context& context, const std::string& objectId );

/*!
 *  Connects to IceGrid/Query interface of the specified registry and gets the
 *  list of objects of type ::orca::Home. If connection fails for some reason,
 *  @p isReachable field of RegistryData is set to FALSE.
 *
 *  When using the default registry, you can obtain the @p locatorString like this:
 *  communicator()->getDefaultLocator()->ice_toString()
 *
 *  If @p tryToPing is TRUE (default), the function attempts to ping each
 *  Home object. Otherwise it doesn't and the @p isReachable field for
 *  each Home is set to TRUE;
 */
RegistryHomeData
getRegistryHomeData( const orcaice::Context& context, const std::string& locatorString, bool tryToPing=true );

/*!
 *  Connects to Home interface of the specified component and gets its
 *  deployment and interface information. If connection fails for some reason,
 *  @p isReachable field of ComponentData is set to FALSE.
 *
 *  If you already have an instance of RegistryData, do this:
 *  getComponentData( context, registryData.adapters[i].name )
 */
ComponentData
getComponentData( const orcaice::Context& context, const orca::FQComponentName& component );

ComponentData
getComponentHomeData( const orcaice::Context& context, const Ice::ObjectPrx& home );

/*!
 *  Convenience function, behaves like the one above.
 */
ComponentData
getComponentData( const orcaice::Context& context, const std::string& adapterId );

/*!
 *  Tries to connect to the specified interface and, if successful, gets its ID.
 *  If connection fails for some reason, the Object ID is set to "unknown".
 */
ProvidesHeader
getProvidesHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName );

/*!
 *  Tries to connect to the specified interface and, if successful, gets its ID.
 *  If connection fails for some reason, the Object ID is set to "unknown".
 */
RequiresHeader
getRequiresHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName );

RegistryHierarchicalData1
home2hierarch1( const RegistryHomeData& registryHomeData );

RegistryHierarchicalData2
home2hierarch2( const RegistryHomeData& registryHomeData, const PlatformHeader& platform, bool tryToPing=true );

//@}
} // namespace

#endif
