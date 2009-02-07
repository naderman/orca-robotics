/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_PRIVATE_UTILITIES_H
#define ORCAICE_PRIVATE_UTILITIES_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <string>

#include <orca/common.h>
#include <orca/ocm.h>
#include <orcaice/context.h>

//  This file contains only internal functions used inside libOrcaIce
//  Don't make Doxygen tags so these functions are not included in the public documentation.

namespace orcaice
{
class Component;
namespace detail
{
// Transfer a property from one property set to another
// returns:
//  0 if it was transferred successfully
//  1 if the property already existed in the target set and it was left untouched
// -1 if the property was not set in the source set, the target was left untouched
int 
transferProperty( const Ice::PropertiesPtr &fromProperties, 
                    const Ice::PropertiesPtr &toProperties,
                    const std::string        &fromKey,
                    const std::string        &toKey,
                    bool                      force );

// Transfer a property from one property set to another
// returns:
//  0 if it was transferred successfully
//  1 if the property already existed in the target set and it was left untouched
int
transferProperty( const Ice::PropertiesPtr& toProperties,
                    const std::string&  fromKey,
                    const std::string&  fromValue,
                    const std::string&  toKey,
                    bool                force );

// Internal helper function.
// behaves like transferProperty. if key is missing, sets the toValue to defaultValue.
void transferPropertyWithDefault( const Ice::PropertiesPtr &fromProperties,
                                    const Ice::PropertiesPtr &toProperties,
                                    const std::string        &fromKey,
                                    const std::string        &toKey,
                                    const std::string        &defaultValue,
                                    bool                      force );

void setFactoryProperties( Ice::PropertiesPtr&properties, const std::string& compTag );

// throws gbxutilacfr::Exception if can't load the file
void setGlobalProperties( Ice::PropertiesPtr& properties, const std::string& filename );

// throws gbxutilacfr::Exception if can't load the file
void setComponentPropertiesFromFile( Ice::PropertiesPtr& properties, const std::string& filename );

// Appends network properties to those currently held by Context.
// throws gbxutilacfr::Exception if anything goes wrong (in particular, failes to connect to the property server)
void setComponentPropertiesFromServer( const Context& context );

/*
TODO: document this
- Sets defaults for component and platform name properties.
    - Converts adapter ID property (if specified) to platform and component names.
    - Replaces empty platform name with 'local'
    - Replaces special platform 'local' with host name.
    - Replaces empty component name with component tag converted to low case
    - Combines platform and component names into adapter ID property.
*/
//     orca::FQComponentName 
void postProcessComponentProperties( const Ice::PropertiesPtr& properties, const std::string& compTag );

// Prints out contents of component's context
void printComponentContext( const orcaice::Context& context );

// Prints out all component properties. Tag is used only for tracing.
void printComponentProperties( const Ice::PropertiesPtr& properties, const std::string& compTag );

// Prints Ice, Orca, and (if not empty) Project version.
// Project version is obatained from the component, this allows for non-orca projects
void printAllVersions( const Component& component );

// Use for 'Application's:
// adds to the set of properties by reading from the component's config file
void addPropertiesFromApplicationConfigFile( Ice::PropertiesPtr   &properties,
                                                const Ice::StringSeq &commandLineArgs,
                                                const std::string    &compTag );

// Use for 'Service's:
// adds to the set of properties by reading from the component's config file
void addPropertiesFromServiceConfigFile( Ice::PropertiesPtr   &properties,
                                            const Ice::StringSeq &commandLineArgs,
                                            const std::string    &compTag );

// adds to the set of properties by reading from the global config file
void addPropertiesFromGlobalConfigFile( Ice::PropertiesPtr   &properties,
                                        const std::string    &compTag );

// Make Home a well-known object, by adding it to the registry
// Catches and ignores CommunicatorDestroyedException.
// Throws various Ice exceptions on failure.
void registerHomeInterface( const Context& context );

} // namespace
} // namespace

#endif
