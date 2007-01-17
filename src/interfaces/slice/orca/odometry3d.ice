/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY_3D_INTERFACE_ICE
#define ORCA2_ODOMETRY_3D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/vehicledescription.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_odometry3d Odometry3d
    @brief Odometry of mobile robots operating in 3D space.
    @{
*/

//! Odometric data structure
struct Odometry3dData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Integrated odometry. The coordinate system has its origin at the point
    //! where the component was initialized.
    Frame3d pose;

    //! Instantanious translational and angular velocities in the vehicle CS.
    //! This means that Vx is forward speed and Vy is side speed
    //! (possible only for some platforms).
    Twist3d motion;
};

//! Data consumer interface.
interface Odometry3dConsumer
{
    //! Transmits the data to the consumer.
    void setData( Odometry3dData obj );
};

/*!
    @brief Access to odometry of mobile robotic bases.
*/
interface Odometry3d
{
    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    nonmutating Odometry3dData getData()
            throws DataNotExistException, HardwareFailedException;
    
    //! Returns description         
    nonmutating VehicleDescription getDescription();

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( Odometry3dConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Odometry3dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
