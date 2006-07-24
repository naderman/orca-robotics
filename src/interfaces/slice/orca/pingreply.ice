/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
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
