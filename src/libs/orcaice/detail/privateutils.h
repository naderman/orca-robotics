/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PRIVATE_UTILITIES_H
#define ORCA2_PRIVATE_UTILITIES_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <string>

#include <orca/orca.h>

//  This file contains only internal functions used inside libOrcaIce
//  Don't make Doxygen tags so these functions are not included in the public documentation.

namespace orcaice
{
class Component;
namespace detail
{
    // Transfer a property from one property set to another
    // returns 0 if it was transferred successfully
    int transferProperty( Ice::PropertiesPtr &fromProperties, Ice::PropertiesPtr &toProperties,
                            const std::string &fromKey, const std::string &toKey, bool force );

    // Internal helper function.
    // behaves like the function above. if key is missing, sets the toValue to defaultValue.
    void transferPropertyWithDefault( Ice::PropertiesPtr &fromProperties, Ice::PropertiesPtr &toProperties,
                        const std::string &fromKey, const std::string &toKey, const std::string &defaultValue, bool force );

    void setFactoryProperties( Ice::PropertiesPtr &properties, const std::string &compTag );

    // throws orcaiceutil::Exception if can't load the file
    void setGlobalProperties( Ice::PropertiesPtr & properties, const std::string & filename );

    // throws orcaiceutil::Exception if can't load the file
    void setComponentProperties( Ice::PropertiesPtr & properties, const std::string & filename );

    /*
     *   Sets the ComponentName parameter.
     *   Replaces special platform 'local' with host name.
     *   Combines platform and component names into adapter ID.
     *
     *   The default for IceStorm server location assumes that one is running
     *   on our host at port 10000.
     *
     *   @note Cannot use Current structure because the component is not fully initialized yet.
     */
    orca::FQComponentName parseComponentProperties( const Ice::CommunicatorPtr &communicator,
                                                    const std::string &compTag );

    // Prints out all component properties. Tag is used only for tracing.
    void printComponentProperties( const Ice::PropertiesPtr &properties, const std::string &compTag );

    // Prints Ice, Orca, and (if not empty) Project version.
    // Project version is obatained from the component, this allows for non-orca projects
    void printAllVersions( const Component& component );
    
} // namespace
} // namespace

#endif
