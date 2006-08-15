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
    @brief Access to IMU devices.
    @{
*/

// define messages first

//! Imu config structure
class ImuConfigData extends OrcaObject
{
    CartesianPoint geometry;
    Frame3d origin;
};

//! The @p imu interface returns calibrated acceleration, gyro and magnetic values 
//! from the IMU sensor. 
class ImuData extends OrcaObject
{
    /** The IMU's raw acceleration value on X-axis. */
    float accelX;
    /** The IMU's raw acceleration value on Y-axis. */
    float accelY;
    /** The IMU's raw acceleration value on Z-axis. */
    float accelZ;
    /** The IMU's raw gyro value on X-axis.         */
    float gyroX;
    /** The IMU's raw gyro value on Y-axis.         */
    float gyroY;
    /** The IMU's raw gyro value on Z-axis.         */
    float gyroZ;
    // maybe need to add these in the interface for some imus
    // with a magnetometer
    // /** The IMU's raw magnetic value on X-axis.     */
    // float magn_x;
    // /** The IMU's raw magnetic value on Y-axis.     */
    //float magn_y;
    // /** The IMU's raw magnetic value on Z-axis.     */
    // float magn_z;
};



/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 *
 */

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

    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

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
