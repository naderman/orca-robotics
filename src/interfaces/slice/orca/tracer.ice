/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACER_INTERFACE_ICE
#define ORCA2_TRACER_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/ocm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_tracer Tracer
    @brief Remote access to component's traces, e.g. errors, warning, etc.
    @{

    The Tracer routes different types of information to different destinations.
*/

//! Types of traced information
enum TraceType {
    //! Information
    InfoTrace,
    //! Warning
    WarningTrace,
    //! Error
    ErrorTrace,
    //! Debug statement
    DebugTrace
};

/*!
    @brief Component status data.
*/
struct TracerData
{   
    //! Time when the trace was produced.
    Time timeStamp;

    //! The fully-qualified name of the interface.
    FQComponentName name;

    //! Trace type.
    TraceType type;

    //! Verbosity level, 1 being the lowest
    int verbosity;

    //! Status message from component.
    string message;
};


/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 */
interface TracerConsumer
{
    //! This is function envoked by the publisher of traces on the consumer of traces.
    void setData( TracerData obj );
};

/*!
    @brief Verbosity levels for different trace types.

    When setting desired verbosity level, any value can be set to -1 to leave the current value unchanged.
*/
struct TracerVerbosityConfig
{   
    //! Verbosity level for error traces.
    int error;

    //! Verbosity level for warning traces.
    int warning;

    //! Verbosity level for information traces.
    int info;

    //! Verbosity level for debug traces.
    int debug;
};

/*!
 *  @brief Routes different types of information to different destinations.
 */
interface Tracer
{
    //! Get current verbosity level for traces transmitted over the network.
    idempotent TracerVerbosityConfig getVerbosity();

    //! Set verbosity level for traces transmitted over the network.
    idempotent void setVerbosity( TracerVerbosityConfig verbosity );

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @see unsubscribe
     */
    void subscribe( TracerConsumer* subscriber )
        throws SubscriptionFailedException;
    
    /*!
     * Unsubscribe an existing subscriber from component messages
     *
     * @see subscribe
     */
    idempotent void unsubscribe( TracerConsumer* subscriber );
};


/*!
    @}
*/
}; // module

#endif
