/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ODOMETRY_3D_INTERFACE_ICE
#define ORCA2_ODOMETRY_3D_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/vehicledescription.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
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
    //! Returns description         
    idempotent VehicleDescription getDescription();

    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    idempotent Odometry3dData getData()
            throws DataNotExistException, HardwareFailedException;
    
    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( Odometry3dConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};


//!  //@}
}; // module

#endif
