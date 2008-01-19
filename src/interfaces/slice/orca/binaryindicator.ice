/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_BINARY_INDICATOR_ICE
#define ORCA2_BINARY_INDICATOR_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup orca_interfaces
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
    ["cpp:const"] idempotent bool getState();

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
