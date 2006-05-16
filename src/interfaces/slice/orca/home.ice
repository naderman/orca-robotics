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

#ifndef ORCA2_HOME_INTERFACE_ICE
#define ORCA2_HOME_INTERFACE_ICE

#include <Ice/BuiltinSequences.ice>

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_home Home
    @brief Navigate Orca component model (OCM)
    @{
*/

/*!
    @brief Contains data on the component's interfaces.
*/
class HomeData extends OrcaObject
{
    //! @brief Names of all provided interfaces as registered with the Locator service.
    //! The Home interface itself is also included in this list.
    Ice::StringSeq provides;
    
    //! Names of all required interfaces as registered with the Locator service.
    Ice::StringSeq requires;

    //! Number of seconds since the start of the component.
    int timeUp;
};

//! Our properties are defined the same way as Ice::PropertyDict.
dictionary<string,string> ComponentProperties;

/*!
    @brief Access to information about components interfaces.
*/
interface Home
{
    //! Returns a listing about the component's interfaces.
    nonmutating HomeData getInterfaces();
    
    //! Returns all component's properties.
    nonmutating ComponentProperties getProperties();
};

/*!
    @}
*/
}; // module

#endif
