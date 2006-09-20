/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMU_INTERFACE_ICE
#define ORCA2_IMU_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_imu Imu
    @brief Access to Inertial Measurement Units.
    @{
*/

// define messages first

//! Imu config structure
class ImuConfigData extends OrcaObject
{
    CartesianPoint geometry;
    Frame3d origin;
};

//! The @p Imu interface returns raw uncallibrated acceleration, gyro and magnetic values 
//! from the IMU sensor. 
class ImuData extends OrcaObject
{
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
    void setData( ImuData obj );
};

/*!
    @brief Access to IMU data.
*/
interface Imu
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating ImuData getData()
            throws HardwareFailedException;

    nonmutating ImuConfigData getConfig();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( ImuConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( ImuConsumer* subscriber );
};


//!  //@}
}; // module

#endif
