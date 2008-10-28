/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_SYSTEMSTATUS_INTERFACE_ICE
#define ORCA_SYSTEMSTATUS_INTERFACE_ICE

#include <orca/status.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_systemstatus SystemStatus
    @brief Access to a system's health and status. A system consists of a bunch of components.
    @{
*/

//! StatusData for a single component
//! Interface provider decides whether it considers the data to be stale
struct ComponentStatusData
{
    bool isDataStale;
    StatusData data;
};

//! Hold the StatusData of an entire system
//! The string contains "component/platform"
dictionary<string,ComponentStatusData> SystemStatusData;

/*!
    Data consumer interface.
 */
interface SystemStatusConsumer
{
    //! ServerPush_Consumer interface
    void setData( SystemStatusData obj );
};

/*!
    @brief Access to a system's status. A system is comprised of a bunch of components.
 */
interface SystemStatus
{
    //! Returns the status of all components of a system
    idempotent SystemStatusData getData()
        throws DataNotExistException;
    
    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( SystemStatusConsumer* subscriber )
        throws SubscriptionFailedException;
    
    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( SystemStatusConsumer* subscriber );
};



/*!
    @}
*/
}; // module

#endif
