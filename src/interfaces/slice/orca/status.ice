/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

//! Possible subsystem states.
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

//! Possible values of subsystem health.
enum SubsystemHealth
{
    //! Subsystem is OK
    SubsystemOk,
    //! Subsystem has encountered an abnormal but non-fault condition
    SubsystemWarning,
    //! Subsystem has encountered a fault
    SubsystemFault
};

//! Status for a single subsystem of a component.
struct SubsystemStatus
{
    //! State in the subsystem's state machine, i.e. what is the subsystem doing?
    SubsystemState state;
    //! Subsystem's health, i.e. how is the subsystem doing?
    SubsystemHealth health;
    //! Human-readable status description
    string message;
    //! Subsystem has not been heard from for an abnormally long time.
    bool isStalled;
    //! OBSOLETE?
    //! Ratio of time since last heartbeat to maximum expected time between heartbeats.
    //! For example, sinceHeartbeat=0.5 means that half of normally expected interval between heartbeats
    //! has elapsed.
    //! Can be negative when maxHeartbeatInterval is negative (indicating that this subsystem is 
    //! not going to send heartbeats at regular periods).
    float sinceHeartbeat;
};

//! Status for all subsystems of a component indexed by subsystem name.
dictionary<string,SubsystemStatus> SubsystemStatusDict;

//! Possible component states.
enum ComponentState
{    
    //! Component is preparing to work, e.g. initialising its resources, etc.
    CompInitialising,
    //! Component is fully initialised and is performing its work
    CompWorking,
    //! Component is preparing to shutdown, e.g. releasing its resources, etc.
    CompFinalising
};

//! Possible values of component health.
enum ComponentHealth
{
    //! All of the component's subsystems are OK
    CompOk,
    //! At least one of the component's subsystems has encountered an abnormal but non-fault condition
    CompWarning,
    //!  At least one of the component's subsystems has encountered a fault
    CompFault
};

//! Status of a single component.
struct ComponentStatus
{
    //! The fully-qualified name of the component.
    FQComponentName name;
    //! Number of seconds since this component was activated.
    int timeUp;
    //! The maximum interval between status updates sent out to a status monitor.
    int publishIntervalSec;
    //! Component state
    ComponentState state;
    //! Component health
    ComponentHealth health;
    //! Is component stalled?
    bool isStalled;
    //! Status of all component's subsystems 
    SubsystemStatusDict subsystems;
};

//! A sequence of component status.
sequence<ComponentStatus> ComponentStatusSeq;

//! A snapshot of component's status.
//! Compared to ComponentStatus, this data structure is more suitable for archiving into a 
//! status history.
struct ComponentStatusEpisode
{
    //! Time when the episode was recorded
    orca::Time timeStamp;
    //! Number of seconds since this component was activated.
    int timeUp;
    //! Component state
    ComponentState state;
    //! Component health
    ComponentHealth health;
    //! Is component stalled?
    bool isStalled;
    //! Status of all component's subsystems 
    SubsystemStatusDict subsystems;

    //! How many times identical status was repeated
    int repeatCount;
};

//! A sequence of component status episodes.
sequence<ComponentStatusEpisode> ComponentStatusEpisodeSeq;

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

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( StatusConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

/*!
    @}
*/
}; // module

#endif
