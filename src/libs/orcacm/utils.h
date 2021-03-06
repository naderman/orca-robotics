/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCACM_COMPONENT_MODEL_LOOKUP_UTILITIES_H
#define ORCACM_COMPONENT_MODEL_LOOKUP_UTILITIES_H

#include <orcaice/context.h>
#include <orcacm/types.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>

namespace orcacm
{
/*!
@name Lookup Functions
 */
//@{

/*!
Attempts to ping the well-known object with specified identity. Returns TRUE if successful.
 */
bool
pingObject( const orcaice::Context& context, const std::string& objectId );

/*!
Same as above but is specialized for interfaces deployed through the component's Admin interface. 
Returns TRUE if successful.
*/
bool 
pingAdminObject( const orcaice::Context& context, const std::string& adminId, const std::string& facetName );

/*!
Connects to IceGrid/Query interface of the specified registry and gets the
list of objects of type ::orca::Home. If connection fails for some reason,
@p isReachable field of RegistryData is set to FALSE.

When using the default registry, you can obtain the @p locatorString like this:
communicator()->getDefaultLocator()->ice_toString().

This function does not try to ping home objects and sets all @c isReachable flags to
TRUE. Use pingHomeObjects() to verify if this is true.
 */
// TODO: split this function into 2: get data, then ping objects.
RegistryHomeData 
getRegistryHomeData( const orcaice::Context& context, const std::string& locatorString );

/*!
Attempts to ping each Home object and set @c isReachable field depending on the outcome.

It's recommended that a pointer to a Stoppable interface is provided (e.g. current thread),
so that a potentially long process can be interrupted. It is safe to pass
a NULL pointer, in which case it is ignored.
*/
void 
pingHomeObjects( RegistryHomeData& data, const orcaice::Context& context, 
                 gbxutilacfr::Stoppable* activity );

/*
OBSOLETE! Use getComponentHomeData()

Connects to Home interface of the specified component and gets its
deployment and interface information. If connection fails for some reason,
@p isReachable field of ComponentData is set to FALSE.

If you already have an instance of RegistryData, do this:
@verbatim
getComponentData( context, registryData.adapters[i].name )
@endverbatim
 */
ComponentData
getComponentData( const orcaice::Context& context, const orca::FQComponentName& component );

/*
Convenience function, behaves like the one above.
 */
ComponentData
getComponentData( const orcaice::Context& context, const std::string& adapterId );

/*!
Connects to Home interface of the specified component and gets its
deployment and interface information. If connection fails for some reason,
@p isReachable field of ComponentData is set to FALSE.

If you already have an instance of RegistryData, do this:
@verbatim
getComponentHomeData( context, registryData.homes[i].proxy )
@endverbatim
 */
ComponentData
getComponentHomeData( const orcaice::Context& context, const Ice::ObjectPrx& home );


/*!
Tries to connect to the specified interface and, if successful, gets its ID.
If connection fails for some reason, the Object ID is set to "unknown".
 */
ProvidesHeader
getProvidesHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName );

/*!
This a special version of the function above, suitable for interfaces which are actually
facets of ADmin interface. We now have 3 such standard interfaces: Tracer, Status, Home.
Tries to connect to the specified interface and, if successful, gets its ID.
If connection fails for some reason, the Object ID is set to "unknown".
 */
ProvidesHeader
getAdminProvidesHeader( const Ice::ObjectPrx& homePrx, const orca::FQInterfaceName& fqName );


/*!
Tries to connect to the specified interface and, if successful, gets its ID.
If connection fails for some reason, the Object ID is set to "unknown".
 */
RequiresHeader
getRequiresHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName );

RegistryHierarchicalData1
home2hierarch1( const RegistryHomeData& registryHomeData );

RegistryHierarchicalData2
home2hierarch2( const RegistryHomeData& registryHomeData, const PlatformHeader& platform, 
                bool tryToPing=true, int tracePing=0 );

//@}
} // namespace

#endif
