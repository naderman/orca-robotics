/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCACM_TYPES_H
#define ORCACM_TYPES_H

#include <vector>
#include <string>
#include <orca/common.h>
#include <orca/ocm.h>

namespace orcacm
{

//! Brief version of operation result
struct ResultHeader
{
    //! Result name
    std::string name;
    //! Result data.
    std::string text;
};

//! Brief version of operation data
//! @see OperationData
struct OperationHeader
{
    //! Operation name, e.g. "getData"
    std::string name;
    //! Operation signature, e.g. "double getData(int)"
    std::string signature;
};

//! Operation data
//! @see OperationHeader
struct OperationData
{
    //! Stringified proxy for the Locator with which the component is registered.
    std::string locatorString;
    //! Parent interface
    orca::FQInterfaceName parent;
    //! Object ID of the parent interface
    std::string parentId;
    //! Operation name, e.g. "getData"
    std::string name;
    //! A listing of stringified results of the operation.
    //! For example, laser scan values converted to text.
    std::vector<ResultHeader> results;
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
    //! Stringified proxy for the Locator with which the component is registered.
    std::string locatorString;
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

//! Brief version of platform data
//! @see PlatformData
struct PlatformHeader
{
    //! Platform name
    std::string name;
};

//! Platform data
//! @see PlatformHeader
struct PlatformData
{
    //! Stringified proxy for the Locator with which component(s) on this platform were registered.
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string adminAddress;
    //! Platform name
    std::string name;
    //! A listing of components
    std::vector<ComponentHeader> components;
};

//! Information about a Home object.
struct HomeHeader
{
    //! Component name
    Ice::ObjectPrx proxy;
    //! Is Home object reachable?
    bool isReachable;
    //! Address of the endpoint on which the Home object was reached.
    std::string address;
};

//! Registry data which lists components
struct RegistryFlatData
{
    //! Stringified proxy for the Locator object
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string address;
    //! Is the registry reachable?
    bool isReachable;
    //! A listing of registered object adapters.
    std::vector<ComponentHeader> adapters;
};

//! Registry data which lists platforms
struct RegistryHierarchicalData1
{
    //! Stringified proxy for the Locator object
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string address;
    //! Is the registry reachable?
    bool isReachable;
    //! A listing of registered object adapters.
    std::vector<PlatformHeader> platforms;
};

//! Registry data which lists platforms
struct RegistryHierarchicalData2
{
    //! Stringified proxy for the Locator object
    std::string locatorString;
    //! Address of the endpoint on which the Admin object was reached.
    std::string address;
    //! Is the registry reachable?
    bool isReachable;
    //! A single platform for which Registry data is listed.
    PlatformHeader platform;
    //! A listing of registered object adapters on a single platform.
    std::vector<HomeHeader> homes;
};

//! Registry data
struct RegistryHomeData
{
    //! Stringified proxy for the Locator object
    std::string locatorString;
    //! Address of the endpoint on which the Query object was reached.
    std::string address;
    //! Is the registry reachable?
    bool isReachable;
    //! A listing of registered object adapters.
    std::vector<HomeHeader> homes;
};

} // namespace

#endif
