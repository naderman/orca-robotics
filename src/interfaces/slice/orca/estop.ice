/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ESTOP_ICE
#define ORCA_ESTOP_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_estop EStop
    @brief Allows emergency-stop if stop signal is received or keep-alive is not received.

There are conceptually two modes in which an E-Stop can operate:
 - Dead-man-switch style: e-stop is activated if a 'keep-going' signal is not received.
 - Active: e-stop is activated iff a 'stop' signal is received.

This interface allows either/both modes of operation.

*/

//! Data sent to consumers who are interested in the server's state.
//! Updates are sent whenever the isEStopActivated state changes.
struct EStopData
{
    //! Time when data was generated.
    Time timeStamp;
    //! true if the vehicle is in the emergency-stopp{ed|ing} mode.
    bool isEStopActivated;
};

interface EStopConsumer
{
    //! Sends data to the consumer.
    void setData( EStopData data );
};

//! Raised when a keep-alive is sent to an estop-disabled platform
exception EStopAlreadyTriggeredException extends OrcaException {};

//! Interface to e-stop.
interface EStop
{
    //! Hit the stop button!
    idempotent void activateEStop();

    //! Keep the thing alive: reset the keep-alive timer to zero.
    //! If the e-stop has been already triggered, keep-alives make no
    //! sense, so an EStopAlreadyTriggeredException is thrown.
    idempotent void keepAlive()
        throws EStopAlreadyTriggeredException;

    //! Returns the required keep-alive period, in seconds.
    //! If a keep-alive is not heard for longer than this, the e-stop
    //! will automatically be activated.
    //! A negative value indicates that keep-alives are not required: the
    //! e-stop can only be activated using 'activateEStop()'.
    idempotent double getRequiredKeepAlivePeriodSec();

    //! If the e-stop was activated, sets the vehicle back to the
    //! operating mode (ie un-sets the e-stop so the vehicle can move again),
    //! and resets the keep-alive timer to zero.
    //! If the vehicle was already in the operating mode, does nothing.
    idempotent void setToOperatingMode();

    //! Returns the latest data. Raises DataNotExistException if data is not available.
    idempotent EStopData getData()
        throws DataNotExistException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( EStopConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */

}; // module

#endif
