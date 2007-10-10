/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_BINARY_SWITCH_ICE
#define ORCA2_BINARY_SWITCH_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_binaryswitch BinarySwitch
    @brief A bank of hardware devices with binary states, e.g. lights.

The BinarySwitch interface provides access to a set of hardware devices which have
binary internal states: on or off. Alarms, flahsing lights are examples of such
devices.

    @{
*/

/*!
    Data for a single binary device.
*/
struct BinaryDeviceData
{
    //! State name
    string name;
    //! State [on/off]
    bool state;
};

/*!
    Data for all binary devices on a robot.
*/
sequence<BinaryDeviceData> BinaryDevicesData;

/*!
    @brief Information about the binary devices of a robot.
*/
struct BinarySwitchData
{
    //! Time when data was measured.
    Time timeStamp;
    //! Binary devices
    BinaryDevicesData devices;
};

//! Interface to a device with a binary state.
interface BinarySwitch
{
    //! Get current state. Raises DataNotExistException if data is not available.
    ["cpp:const"] idempotent BinarySwitchData getData()
        throws DataNotExistException, HardwareFailedException;

    //! Set current state
    idempotent void setState( int device, bool state )
        throws MalformedParametersException, HardwareFailedException;

    //! Set current state, then after a timeout return to previous state
    void timedSetState( int device, bool state, long timeoutMs )
        throws MalformedParametersException, HardwareFailedException;

    //! Toggle current state
    void toggleState( int device )
        throws MalformedParametersException, HardwareFailedException;

    //! Toggle state, then after a timeout toggle it back
    void timedToggleState( int device, long timeoutMs )
        throws MalformedParametersException, HardwareFailedException;
};

/*! @} */
}; // module

#endif
