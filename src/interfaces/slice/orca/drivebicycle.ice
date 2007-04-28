/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRIVE_BICYCLE_INTERFACE_ICE
#define ORCA2_DRIVE_BICYCLE_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/vehicledescription.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_drivebicycle DriveBicycle
    @brief Controls a mobile robot with bicycle steering.
    @{
*/

//! Bicycle steering/velocity command.
struct DriveBicycleData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Angle of the steering wheel [rad]
    double steerAngle;

    //! Forward speed [m/s]. Negative for reverse.
    double speed;
};

//! Data consumer interface.
interface DriveBicycleConsumer
{
    //! Transmits the data to the consumer.
    void setData( DriveBicycleData obj );
};


/*!
    Controls a mobile robot with bicycle steering.
*/
interface DriveBicycle
{
    //! Get vehicle description
    ["cpp:const"] idempotent VehicleDescription getDescription();

    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    ["cpp:const"] idempotent DriveBicycleData getData()
            throws DataNotExistException, HardwareFailedException;
    
    //! Set velocity command
    idempotent void setCommand( DriveBicycleData data )
            throws HardwareFailedException;    

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( DriveBicycleConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( DriveBicycleConsumer* subscriber );

};

//! @}
}; // module

#endif
