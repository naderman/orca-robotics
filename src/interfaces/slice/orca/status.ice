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
#include <IceStorm/IceStorm.ice>

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

//! Possible component states
enum ComponentState
{    
    //! Component is preparing to work, e.g. initialising its resources, etc.
    CompInitialising,
    //! Component is fully initialised and is performing its work
    CompActive,
    //! Component is preparing to shutdown, e.g. releasing its resources, etc.
    CompFinalising
};

//! Possible component health values
enum ComponentHealth
{
    //! All of the component's subsystems are OK
    CompOk,
    //! At least one of the component's subsystems has encountered an abnormal but non-fault condition
    CompWarning,
    //!  At least one of the component's subsystems has encountered a fault
    CompFault,
    //!  At least one of the component's subsystems has not been heard from for an abnormally long time
    CompStalled
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


//! Status for all subsystems of a component
dictionary<string,SubsystemStatus> SubsystemStatusDict;

//! Status of a single component
struct ComponentStatus
{
    //! The fully-qualified name of the component.
    FQComponentName name;
    //! Number of seconds since this component was activated.
    int timeUp;
    //! How often can you expect to receive a ComponentStatus msg?
    double publishIntervalSec;
    //! Current state of the component, see above
    ComponentState state;
    //! Current health of the component, see above
    ComponentHealth health;
    //! Status of all component subsystems 
    SubsystemStatusDict subsystems;
};


/*!
    @brief Component status data
*/
struct StatusData
{
    //! Time when status data was recorded
    Time timeStamp;
    //! The status of the component
    ComponentStatus compStatus;
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
    @brief Access to a component's status
 */
interface Status
{
    //! Returns the latest status.
    idempotent StatusData getData()
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

    //
    // experimental!
    //
    idempotent IceStorm::Topic* topic();
};

/*!
    @}
*/
}; // module

#endif
