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

#ifndef ORCA2_ORCA_ICE
#define ORCA2_ORCA_ICE

/*!
    @brief namespace for the Orca project
    @namespace orca

    This namespace defines the core reusable part of Orca: interfaces and objects.
*/
module orca
{

/*!
    @brief Unix absolute time
*/
struct Time
{
    //! Number of seconds.
    int seconds;
    //! Number of microseconds.
    int useconds;
};

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

/*!
    @brief Base class for all data types in Orca.
    
    Deriving from a single class allows polymorphism.
    
    @note An Orca @e object refers to a piece of data whereas an Ice @e object which
    refers to a remote servant.
*/
class OrcaObject
{
    //! Time when the object was observed, created, etc.
    Time timeStamp;
};

//! A byte array which can store any information.
sequence<byte> ByteSequence;

/*!
    @brief A generic run-time exception.
    
    Orca Components can use this to signal error conditions to their clients.
*/
exception OrcaException
{
    //! Error description.
    string what;
};

//! Server failed to configure itself as requrested by client.
exception ConfigurationNotExistException extends OrcaException {};

/*!
    Raised when the server does not have the requested data.

    Typically, this is because the server has not fully initialized yet.
*/
exception DataNotExistException extends OrcaException {};

//! Indicates a problem with robot hardware, e.g. sensors and actuators.
exception HardwareFailedException extends OrcaException {};

//! Raised when the server fails to subscribe client for periodic updates.
exception SubscriptionFailedException extends OrcaException {};

}; // module

#endif
