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

#ifndef ORCA2_BINARY_INDICATOR_ICE
#define ORCA2_BINARY_INDICATOR_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_binaryindicator BinaryIndicator
    @brief Hardware device with a binary state, e.g. flashing lights.

The BinaryIndicator interface provides access to a hardware device which has
a binary internal state: on or off. Alarms, flahsing lights are examples of such
devices.

    @{
*/

//! Interface to a device with a binary state.
interface BinaryIndicator
{
    //! Get current state
    nonmutating bool getState();

    //! Set current state
    idempotent void setState( bool state );

    //! Set current state, then after a timeout return to previous state
    void timedSetState( bool state, long timeoutMs );

    //! Toggle current state
    void toggleState();

    //! Toggle state, then after a timeout toggle it back
    void timedToggleState( long timeoutMs );
};

/*! @} */
}; // module

#endif
