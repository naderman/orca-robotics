/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    //! Observed state: component's Home interface cannot be reached
    ObsCompInactive,
    //! Observed state: in the process of connecting to component's Status interface
    ObsCompConnecting,
    //! Reported state: component is preparing to work, e.g. initialising its resources, etc.
    ObsCompInitialising,
    //! Reported state: component is fully initialised and is performing its work
    ObsCompActive,
    //! Reported state: component is preparing to shutdown, e.g. releasing its resources, etc.
    ObsCompFinalising
};

//! Possible component health values
enum ObservedComponentHealth
{
    //! All of the component's subsystems are OK
    ObsCompOk,
    //! At least one of the component's subsystems has encountered an abnormal but non-fault condition
    ObsCompWarning,
    //! At least one of the component's subsystems has encountered a fault
    ObsCompFault,
    //! At least one of the component's subsystems has not been heard from for an abnormally long time
    ObsCompStalled,
    //! Component has not been heard from for an abnormally long time. Can only be observed from outside the component.
    ObsCompStale
};

//! StatusData for a single component
struct ObservedComponentStatus
{
    //! The fully-qualified name of the component.
    FQComponentName name;
    //! Number of seconds since this component was activated.
    int timeUp;
    //! How often can you expect to receive an ObservedComponentStatus msg?
    double publishIntervalSec;
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
    //! Time when SystemStatusData was recorded
    Time timeStamp;
    //! How often can you expect to receive a SystemStatusData msg?
    double publishIntervalSec;
    //! The status of system composed of components
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

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( SystemStatusConsumer* subscriber )
        throws SubscriptionFailedException;
};



/*!
    @}
*/
}; // module

#endif
