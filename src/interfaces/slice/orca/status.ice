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

//! Possible subsystem status values
enum SubsystemStatusType
{
    //! Subsystem is initialising -- it's not exactly OK yet, but there's also no fault yet.
    SubsystemStatusInitialising,
    //! Subsystem is OK
    SubsystemStatusOk,
    //! Subsystem has encountered an abnormal but non-fault condition
    SubsystemStatusWarning,
    //! Subsystem has declared a fault
    SubsystemStatusFault,
    //! Subsystem has not been heard from for an abnormally long time
    SubsystemStatusStalled
};

//! Status for a single subsystem of this component
struct SubsystemStatus
{
    //! Machine-readable status description
    SubsystemStatusType type;

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
