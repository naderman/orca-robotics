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

/*!
The states of a component as seen by a remote monitor.

Inactive --> Reporting <--> Stale
    ^           ^_|           |
    |_________________________|

The following represents the same state machine in the format of State Machine Compiler (see smc.sf.net) : 
@verbatim
Inactive
{
    received
    Reporting
    {}
}

Reporting
{
    timedOut
    Stale
    {}

    received
    nil
    {}
}

Stale
{
    received
    Reporting
    {}

    gaveUp
    Inactive
    {}
}
@endverbatim   
*/
enum EstimatedComponentState
{    
    //! The component is inactive. This can happen for two reasons which we cannot distinguish:
    //! - the component is deactivated, i.e. does not actually execute, or
    //! - it is activated but is unreachable from the location of the monitor.
    EstCompInactive,
    //! The monitor is connected to the component's Status interface and is receiving regular
    //! status updates.
    EstCompReporting,
    //! The monitor is connected to the component's Status interface but has stopped receiving 
    //! regular updates. Typically, the monitor will try to re-establish a connection to the
    //! monitored component.
    EstCompStale
};

//! Remotely estimated status of a component.
struct EstimatedComponentStatus
{
    //! Fully-qualified name of the component.
    FQComponentName name;

    //! The estimate of the remote component's state performed by the monitor.
    EstimatedComponentState estimatedState;

    //! The contents of the reported status depend on the current estimated state:
    //! - EstCompInactive
    //!     - not defined
    //! - EstCompReporting
    //!     - the latest reported status
    //! - EstCompStale
    //!     - the last reported status before status updates stopped arriving.
    ComponentStatusSeq reportedStatus;
};

//! A sequence of EstimatedComponentStatus
sequence<EstimatedComponentStatus> EstimatedComponentStatusSeq;

//! Holds the estimated component status of an entire system.
//! The dictionary contains a sequence of EstimatedComponentStatus indexed by the platform name,
//! i.e. each platform may have several components.
dictionary<string,EstimatedComponentStatusSeq> SystemStatusDict;


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
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

/*!
    @}
*/
}; // module

#endif
