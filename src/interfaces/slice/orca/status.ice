/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_STATUS_INTERFACE_ICE
#define ORCA2_STATUS_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/ocm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_status Status
    @brief Access to a component's health and status subsystem
    @{
*/

//! Possible subsystem states
enum SubsystemState
{    
    //! Subsystem has been created but has not started initialisation process.
    SubsystemIdle,
    //! Subsystem is preparing to work, e.g. initialising its resources, etc.
    SubsystemInitialising,
    //! Subsystem is fully initialised and is performing its function.
    SubsystemWorking,
    //! Subsystem is preparing to shutdown, e.g. releasing its resources, etc.
    SubsystemFinalising,
    //! Subsystem is not longer functioning.
    SubsystemShutdown
};

//! Possible subsystem status values
enum SubsystemHealth
{
    //! Subsystem is OK
    SubsystemOk,
    //! Subsystem has encountered an abnormal but non-fault condition
    SubsystemWarning,
    //! Subsystem has encountered a fault
    SubsystemFault,
    //! Subsystem has not been heard from for an abnormally long time
    SubsystemStalled
};

//! Status for a single subsystem of a component.
struct SubsystemStatus
{
    //! Current state in the subsystem's state machine. I.e. what is the subsystem doing?
    SubsystemState state;

    //! Subsystem's health. I.e. how is the subsystem doing?
    SubsystemHealth health;

    //! Human-readable status description
    string message;

    //! Ratio of time since last heartbeat to maximum expected time between heartbeats.
    //! For example, sinceHeartbeat=0.5 means that half of normally expected interval between heartbeats
    //! has elapsed.
    float sinceHeartbeat;
};

//! Status for all subsystems of this component
dictionary<string,SubsystemStatus> SubsystemsStatus;

/*!
    @brief Component status data.
*/
struct StatusData
{
    //! Time when status was recorded
    Time timeStamp;

    //! The fully-qualified name of the component.
    FQComponentName name;

    //! Number of seconds since this component was activated.
    int timeUp;

    //! Status of component subsystems
    SubsystemsStatus subsystems;
};


/*!
    Data consumer interface.
 */
interface StatusConsumer
{
    //! ServerPush_Consumer interface
    void setData( StatusData obj );
};

/*!
    @brief Access to a robot's status subsystem
 */
interface Status
{
    //! Returns the latest status.
    ["cpp:const"] idempotent StatusData getData()
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
    void subscribe( StatusConsumer* subscriber )
            throws SubscriptionFailedException;
    
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
