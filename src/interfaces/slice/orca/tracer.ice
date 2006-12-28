/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACER_INTERFACE_ICE
#define ORCA2_TRACER_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_tracer Tracer
    @brief Remote access to component's traces, e.g. errors, warning, etc.
    @{

    The Tracer routes different types of information to different destinations.
*/

/*!
    @brief Component status data.
*/
struct TracerData
{   
    //! Time when the trace was produced.
    Time timeStamp;

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
 *  @brief Routes different types of information to different destinations.
 */
interface Tracer
{
    //! Set verbosity level for traces transmitted over the network.
    //! Set to -1 to leave unchanged.
    idempotent void setVerbosity( int error, int warn, int info, int debug );

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
     * Unsubscribe an existing subscriber.
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
