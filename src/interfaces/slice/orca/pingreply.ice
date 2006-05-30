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

#ifndef ORCA2_PING_REPLY_INTERFACE_ICE
#define ORCA2_PING_REPLY_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_pingreply Ping/Reply
    @brief Utility interfaces for performance testing.
    @{
*/

/*!
    @brief Test component which initiates timed transmission.
*/
interface Pinger
{
    //! Excepts data returned in the response to calling ping1 operation of Replier.
    void callback( OrcaObject data );
};

/*!
    @brief Test component which replies to timed transmission.
*/
interface Replier
{
    //! Specifies the Pinger proxy, only required for tests with ping1.
    idempotent void setPinger( Pinger* server );

    //! Sends data one way and expects it to be returned using callback operation
    //! of Pinger interface.
    void ping1( OrcaObject data );

    //! Sends data one way and receives it back.
    OrcaObject ping2( OrcaObject data );

    //! Initiate shutdown.
    idempotent void shutdown();
};

/*!
    @}
*/
}; // module

#endif
