/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POSITION_3D_INTERFACE_ICE
#define ORCA2_POSITION_3D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_position3d Position3d
    @brief Odometry of 3D mobile robots. @b OBSOLETE!
    @note OBSOLETE! Use orca::Odometry3d interface instead.
    @{
*/

// define messages first

//! Position3d data structure
struct Position3dData
{
    //! Time when data was measured.
    Time timeStamp;
    //! Robot pose in global CS according to odometry.
    Frame3d pose;
    //! Translational and angular velocities in the robot CS.
    //! This means that Vx is forward speed, Vy is side speed, and Vz is 
    //! vertical speed. Rate of change of angular position is designated by
    //! the axis around which the rotation happens. So, roll rate is Wx, 
    //! pitch rate is Wy, and yaw rate is Wz.
    Twist3d motion;
};

//! Position3d description data structure
struct Position3dDescription
{
    //! Time when data was measured.
    Time timeStamp;
    //! Pose of the robot base, in the robot CS
    Frame3d offset;
    //! Dimensions of the base
    Size3d size;
};


/*!
 * Data consumer interface (needed only for the push pattern).
 */
interface Position3dConsumer
{
    //! Transmits the data to the consumer.
    void setData( Position3dData obj );
};

/*!
    @brief Access to odometry of 3d mobile robitic bases.
*/
interface Position3d
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating Position3dData getData()
            throws DataNotExistException, HardwareFailedException;

    //! Returns description of the position device.
    nonmutating Position3dDescription getDescription();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( Position3dConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Position3dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
