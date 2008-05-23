/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_BUTTON_ICE
#define ORCA2_BUTTON_ICE

#include <orca/common.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_button Button
    @brief A simple stateless interface which can be 'pressed' remotely.

The Button interface is a simple thing which can be 'pressed'
remotely.  It provides no feedback whatsoever.

    @{
*/

//! Interface to a device with a binary state.
interface Button
{
    //! Press
    void press();
};

/*! @} */
}; // module

#endif
