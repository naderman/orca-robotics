/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_ORCA_COMPONENT_MODEL_ICE
#define ORCA2_ORCA_COMPONENT_MODEL_ICE

module orca
{

/*!
    @brief Fully qualified executable name.
*/
struct FQExecutableName
{
    //! Name of the executable file
    string executable;
    //! Host name
    string host;
};

/*!
    @brief Fully qualified component name.
    @see FQInterfaceName, FQTopicName
*/
struct FQComponentName
{
    //! Platform name
    string platform;
    //! Component name
    string component;
};

/*!
    @brief Fully qualified interface name.
    Usually represented in a single string as 'iface\@platform/component'

    @note Cannot use member variable 'interface', it's reserved by Ice.
    @see FQComponentName, FQTopicName
*/
struct FQInterfaceName
{
    //! Platform name
    string platform;
    //! Component name
    string component;
    //! Interface name
    string iface;
};

/*!
    @brief Fully qualified Ice Storm topic name.
    The idea is that the same interface can publish to multiple topics corresponding
    different data streams.
    @see FQInterfaceName, FQComponentName
*/
struct FQTopicName
{
    //! Platform name
    string platform;
    //! Component name
    string component;
    //! Interface name
    string iface;
    //! Topic name
    string topic;
};

//! Brief version of provided interface data
struct ProvidedInterface
{
    //! Interface name
    string name;
    //! Object ID of the interface
    string id;
};

//! Brief version of required interface data
struct RequiredInterface
{
    //! Interface name
    FQInterfaceName name;
    //! Object ID of the interface
    string id;
};

//! All provided interfaces
sequence<ProvidedInterface> ProvidesInterfaces;

//! All required interfaces
sequence<RequiredInterface> RequiresInterfaces;

//! Component data
struct ComponentData
{
    //! Component name
    orca::FQComponentName name;
    //! A listing of provided interfaces
    ProvidesInterfaces provides;
    //! A listing of required interfaces
    RequiresInterfaces requires;
};

}; // module

#endif
