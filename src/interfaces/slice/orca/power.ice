/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POWER_INTERFACE_ICE
#define ORCA2_POWER_INTERFACE_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_power Power
    @brief Power subsystem
    @{
*/

//! Battery charging state
enum ChargingState {
    //! Battery is currently charging
    ChargingYes,
    //! Battery is currently discharging
    ChargingNo,
    //! Battery charge state is not known
    ChargingUnknown
};


/*!
    Data for a single battery.
*/
struct BatteryData
{
    //! Battery name
    string name;
    //! Battery voltage [V]
    float voltage;
    //! Percent of full charge [%]
    short percent;
    //! Battery charging state (see above)
    ChargingState isBatteryCharging;
    //! Battery life remaining when battery is decharging [s],
    //! i.e. ChargingState is ChargingNo.
    //! Undefined when ChargingState is different from ChargingNo.
    int secRemaining;
};

/*!
    Data for all batteries on a robot.
*/
sequence<BatteryData> BatteryDataSeq;

/*!
    @brief Information about the power subsystem of a robot.
*/
struct PowerData
{
    //! Time when data was measured.
    Time timeStamp;
    //! Battery information
    BatteryDataSeq batteries;
};

/*!
    @brief Interface to the consumer of power information.
*/
interface PowerConsumer
{
    //! Sends data to the consumer
    void setData( PowerData obj );
};

/*!
    @brief Access to a robot's power subsystem
*/
interface Power
{
    //! Returns the latest data. Raises DataNotExistException if data is not available. Raises 
    //! HardwareFailedException there is a problem with the underlying hardware.
    idempotent PowerData getData()
        throws DataNotExistException, HardwareFailedException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( PowerConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};


/*!
    @}
*/
}; // module

#endif
