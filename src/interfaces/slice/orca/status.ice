/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_STATUS_INTERFACE_ICE
#define ORCA2_STATUS_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_status Status
    @brief Access to a component's health and status subsystem
    @{
*/

/*!
    @brief Component status data.
*/
class StatusData extends OrcaObject
{
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
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 *
 */
interface StatusConsumer
{
    //! ServerPush_Consumer interface
    void setData( StatusData obj );
};

/*!
 *  @brief Access to a robot's status subsystem
 */
interface Status
{
    //! Returns the latest status.
    nonmutating StatusData getData();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( StatusConsumer* subscriber );
    
    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);
    
    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( StatusConsumer* subscriber );
};


/*!
    @}
*/
}; // module

#endif
