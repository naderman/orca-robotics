/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PING_REPLY_INTERFACE_ICE
#define ORCA2_PING_REPLY_INTERFACE_ICE

// #include <orca/common.ice>

module orca
{
module util
{

/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_pingreply Ping/Reply
    @brief Utility interfaces for latency testing.

    This slice interface is outside of the system for compiling and building Orca
    interfaces. It has to be compiled manually and the resulting files copied to into
    @c util/replier directory.

@verbatim
$ slice2cpp pingreply.ice -I../../interfaces/slice -I/usr/share/slice
@endverbatim
    @{
*/

//! An array of doubles.
sequence<double> DoubleSequence;

struct PingReplyData
{   
    //! Sequential number to keep track of lost objects and out-of-sequence arrivals
    int id;

    //! Payload of the ping object. We use doubles to make Ice work a bit harder 
    //! during marshalling/demarshalling. Presumably, a sequence of bytes can be 
    //! copied all at once, whereas doubles must be converted according to endiness. 
    //! (This is just a theory, it should be verified).
    DoubleSequence doubles;
};

/*!
    @brief Test component which initiates timed transmission.
*/
interface LatencyPinger
{
    //! The Replier will call this function to return the ping object.
    void takeItBack( PingReplyData data );
};

/*!
    @brief Test component which replies to timed transmission.
*/
interface LatencyReplier
{
    //! Specifies the LatencyPinger proxy, only required for tests with ping1().
    idempotent void setPinger( LatencyPinger* server );

    //! Sends data one way and expects it to be returned using callback operation
    //! of LatencyPinger interface.
    void takeIt( PingReplyData data );

    //! Sends data one way and receives it back.
    PingReplyData takeItAndReturn( PingReplyData data );

    //! Initiate shutdown.
    idempotent void shutdown();
};

/*!
    @}
*/
}; // module
}; // module

#endif
