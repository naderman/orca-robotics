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

#ifndef ORCA2_STATUS_INTERFACE_ICE
#define ORCA2_STATUS_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_status Status
    @brief Access to a component's health and status subsystem
    @{
*/

/*!
    @brief Component status data.
*/
class StatusData extends OrcaObject
{
    //! The fully-qualified name of the interface.
    FQComponentName name;
    //! Message category, e.g. warning, error, etc.
    string category;
    //! Verbosity level, 1 being the lowest
    int verbosity;
    //! Status message from component.
    string message;
};

/*!
    @brief Access to a robot's status subsystem
*/
interface Status
{
    //! ClientPull_Supplier interface
    nonmutating StatusData getStatus();
};

/*!
    @brief Interface to the consumer of status information.
*/
interface StatusConsumer
{
    //! ServerPush_Consumer interface
    void setStatus( StatusData obj );
};


/*!
    @}
*/
}; // module

#endif
