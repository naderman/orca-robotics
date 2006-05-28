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
    @brief Pinger.
*/
interface Pinger
{
    void callback( OrcaObject data );
};

/*!
    @brief Replier.
*/
interface Replier
{
    idempotent void initiateCallback( Pinger* server );

    void ping1( OrcaObject data );

    OrcaObject ping2( OrcaObject data );

    idempotent void shutdown();
};

/*!
    @}
*/
}; // module

#endif
