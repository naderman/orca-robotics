/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCACM_TYPES_H
#define ORCACM_TYPES_H

#include <vector>
#include <string>
#include <Ice/BuiltinSequences.h>
#include <orca/orca.h>

namespace orcacm
{

//! Brief version of operation data
//! @see OperationData
struct OperationHeader
{
    //! Operation name
    std::string name;
};

//! Operation data
//! @see OperationHeader
struct OperationData
{
    //! Parent interface
    orca::FQInterfaceName parent;
    //! Object ID of the parent interface
    std::string parentId;
    //! Operation name
    std::string name;
};

//! Brief version of provided interface data
//! @see RequiresHeader, InterfaceData
struct ProvidesHeader
{
    //! Interface name
    std::string name;
    //! Object ID of the interface
    std::string id;
    //! Is the interface reachable?
    bool isReachable;
};

//! Brief version of required interface data
//! @see ProvidesHeader, InterfaceData
struct RequiresHeader
{
    //! Interface name
    orca::FQInterfaceName name;
    //! Object ID of the interface
    std::string id;
    //! Is the interface reachable? (The interface on the remote object).
    bool isReachable;
};

//! Interface data
//! @see ProvidesHeader, RequiresHeader
struct InterfaceData
{
    //! Interface name
    orca::FQInterfaceName name;
    //! Object ID of the interface
    std::string id;
    //! A listing of supported operations
    std::vector<OperationHeader> operations;
};

//! Brief version of component data
//! @see ComponentData
struct ComponentHeader
{
    //! Component name
    orca::FQComponentName name;
    //! Is the component reachable?
    bool isReachable;
    //! Address of the endpoint on which the component was reached.
    std::string address;
};

//! Component data
//! @see ComponentHeader
struct ComponentData
{
    //! Stringified proxy for the Locator with which the component is registered.
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string adminAddress;
    //! Component name
    orca::FQComponentName name;
    //! Is the component reachable?
    bool isReachable;
    //! Address of the endpoint on which the component was reached.
    std::string address;
    //! Time since activation [seconds].
    int timeUp;
    //! A listing of provided interfaces
    std::vector<ProvidesHeader> provides;
    //! A listing of required interfaces
    std::vector<RequiresHeader> requires;
};

//! Registry data
struct RegistryData
{
    //! Stringified proxy for the Locator object
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string adminAddress;
    //! Is the registry reachable?
    bool isReachable;
    //! A listing of registered object adapters.
    std::vector<ComponentHeader> adapters;
};

//! Registry data
struct RegistryHomeData
{
    //! Stringified proxy for the Locator object
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string adminAddress;
    //! Is the registry reachable?
    bool isReachable;
    //! A listing of registered Home objects.
    Ice::ObjectProxySeq homes;
};

} // namespace

#endif
