/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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
    Unix absolute time
*/
struct Time
{
    int seconds;
    int useconds;
};


/*!
    Fully qualified component name.
    @see FQInterfaceName, FQTopicName
*/
struct FQComponentName
{
    string platform;
    string component;
};

/*!
    Fully qualified interface name.
    @note Cannot use member variable 'interface', it's reserved by Ice.
    @see FQComponentName, FQTopicName
*/
struct FQInterfaceName
{
    string platform;
    string component;
    string iface;
};

/*!
    Fully qualified Ice Storm topic name.
    @see FQInterfaceName, FQComponentName
*/
struct FQTopicName
{
    string platform;
    string component;
    string iface;
    string topic;
};

/*!
    All objects are derived from this class to allow polymorphism.
    @note Do we need the timestamp?
*/
class OrcaObject
{
    Time timeStamp;
};

/*!
    A time-stamped string.
*/
class StringObj extends OrcaObject
{
    string str;
};

//! A sequence of string objects.
sequence<StringObj> StringObjSequence;

// NOTE: Do we need something like this? Not using it yet.
//interface OrcaComponent
//{
//    // Admin
//    void shutdown();
//};

//! A byte array which can store any information.
sequence<byte> ByteSequence;

}; // module

#endif
