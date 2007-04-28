/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POWER_INTERFACE_ICE
#define ORCA2_POWER_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_power Power
    @brief Power subsystem
    @{
*/

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
    float percent;
    //! Battery life remaining
    float secRemaining;
};

/*!
    Data for all batteries on a robot.
*/
sequence<BatteryData> BatteriesData;

/*!
    @brief Information about the power subsystem of a robot.
*/
struct PowerData
{
    //! Time when data was measured.
    Time timeStamp;
    //! Battery information
    BatteriesData batteries;
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
    ["cpp:const"] idempotent PowerData getData()
        throws DataNotExistException, HardwareFailedException;

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( PowerConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( PowerConsumer* subscriber );
};


/*!
    @}
*/
}; // module

#endif
