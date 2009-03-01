/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Ben Upcroft, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMU_INTERFACE_ICE
#define ORCA2_IMU_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_imu Imu
    @brief Access to Inertial Measurement Units.
    @{
*/

// define messages first

//! Imu configuration objectd
struct ImuDescription
{
    //! Time when data was measured.
    Time timeStamp;

    //! Offset of the sensor with respect to the robot,
    //! in the robot local coordinate system.
    Frame3d offset; 

    //! Dimensions of the sensor
    Size3d  size;
};

//! The @p Imu interface returns raw uncallibrated acceleration, gyro and magnetic values 
//! from the IMU sensor. 
struct ImuData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Components of acceleration.
    CartesianAcceleration accel;

    //! Components of angular velocity.
    AngularVelocity gyro;

    // maybe need to add these in the interface for some imus
    // with a magnetometer

    // Components of the IMU's raw magnetic value.
    // CartesianVector magnet;
};


/*!
 * Imu PVA state information consumer interface
 */
interface ImuConsumer
{
    //! Transmits the data to the consumer.
    void setData( ImuData obj );
};

/*!
    @brief Access to IMU data.
*/
interface Imu
{
    //! Returns device description.
    idempotent ImuDescription getDescription();

    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    idempotent ImuData getData()
            throws HardwareFailedException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( ImuConsumer* subscriber )
        throws SubscriptionFailedException;
};


//!  //@}
}; // module

#endif
