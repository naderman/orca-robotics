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

//! Possible component states including observed states
enum ObservedComponentState
{    
    //! Component has been created but has not setup its interfaces yet
    ObsCompInactive,
    //! Observed state: can the component be connected to?
    ObsCompConnecting,
    //! Component is preparing to work, e.g. initialising its resources, etc.
    ObsCompInitialising,
    //! Component is fully initialised and is performing its work.
    ObsCompActive,
    //! Component is preparing to shutdown, e.g. releasing its resources, etc.
    ObsCompFinalising,
    //! Observed state: can the component still be "reached"?
    ObsCompDisconnecting
};

//! Possible component health values
enum ObservedComponentHealth
{
    //! All of the component's subsystems are OK
    ObsCompOk,
    //! At least one of the component's subsystems has encountered an abnormal but non-fault condition
    ObsCompWarning,
    //!  At least one of the component's subsystems has encountered a fault
    ObsCompFault,
    //!  At least one of the component's subsystems has not been heard from for an abnormally long time
    ObsCompStalled,
    //! Component has not been heard from for an abnormally long time
    //! Can only be observed from "outside"
    ObsCompStale
};

//! StatusData for a single component
struct ObservedComponentStatus
{
    //! The fully-qualified name of the component.
    FQComponentName name;
    //! Number of seconds since this component was activated.
    int timeUp;
    //! Observed state of the component, see above
    ObservedComponentState state;
    //! Observed health of the component, see above
    ObservedComponentHealth health;
    //! Status of all component subsystems 
    SubsystemStatusDict subsystems;
};

//! A sequence of ComponentStatus
sequence<ObservedComponentStatus> ObservedComponentStatusSeq;

//! Hold the observed component status of an entire system
//! The dictionary contains a sequence of ObservedComponentStatus indexed by the platform name
dictionary<string,ObservedComponentStatusSeq> SystemStatusDict;

//! The data corresponding to the SystemStatus interface
struct SystemStatusData
{
    //! Time when status data was recorded
    Time timeStamp;
    
    SystemStatusDict systemStatus;
};

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
