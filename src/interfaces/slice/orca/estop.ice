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

struct EStopDescription
{
    //! Time when description was generated.
    Time timeStamp;

    //! If you don't hear from me for this long, you should stop.
    double keepAlivePeriodSec;

    //! Human-readable information describing the source of the e-stop.
    string info;
};

//! Data sent to consumers who are interested in the server's state.
//! Updates are sent whenever the isEStopActivated state changes.
struct EStopData
{
    //! Time when data was generated.
    Time timeStamp;
    //! true if the vehicle is in the emergency-stopp{ed|ing} mode.
    bool isEStopActivated;
    //! Set to false if the e-stop is disabled (e.g. manual override to ignore the e-stop)
    //! A disabled e-stop never sets isEStopActivated.
    bool isEStopEnabled;
    //! Human-readable information, only filled out if the e-stop is activated
    string info;
};

interface EStopConsumer
{
    //! Sends data to the consumer.
    void setData( EStopData data );
};

//! Interface to e-stop.
interface EStop
{
    //! Returns device description
    idempotent EStopDescription getDescription();

    //! Returns the latest data.
    idempotent EStopData getData()
        throws DataNotExistException;

    //! Allows disabling of the e-stop (e.g. manual override to ignore an e-stop).
    //! A disabled e-stop will never activate.
    void setEnabled( bool enabled );

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( EStopConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

/*! @} */

}; // module

#endif
