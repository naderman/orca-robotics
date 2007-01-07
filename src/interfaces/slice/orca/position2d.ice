/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POSITION_2D_INTERFACE_ICE
#define ORCA2_POSITION_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_position2d Position2d
    @brief Odometry of planar mobile robots.
    @{
*/

// define messages first

//! Position2d data structure
struct Position2dData
{
    //! Time when data was measured.
    Time timeStamp;
    //! Robot pose in global CS according to odometry.
    Frame2d pose;
    //! Translational and angular velocities in the robot CS.
    //! This means that Vx is forward speed and Vy is side speed
    //! (possible only for some platforms).
    Twist2d motion;
    //! Are the motors stalled
    bool stalled;
};

//! Position2d geometry data structure
struct Position2dGeometry
{
    //! Time when data was measured.
    Time timeStamp;
    //! Pose of the robot base, in the robot CS
    Frame2d offset;
    //! Dimensions of the base
    Size2d size;
};


/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 *
 */
interface Position2dConsumer
{
    //! Transmits the data to the consumer.
    void setData( Position2dData obj );
    
    // Do we need it here? Or if someone needs geometry they should pull it
    // with getGeometry(). In that case, do we rename this object Position2dDataConsumer?
    //void setGeometry( Position2dGeometry obj );
};

/*!
    @brief Access to odometry of 2d mobile robotic bases.
*/
interface Position2d
{
    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    //! May raise HardwareFailedException if there is some problem with hardware.
    nonmutating Position2dData getData()
            throws DataNotExistException, HardwareFailedException;

    // Returns the current configuration.
    //nonmutating Position2dConfig getConfig();
    
    //! Returns geometry of the position device.            
    nonmutating Position2dGeometry getGeometry();

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( Position2dConsumer* subscriber )
            throws SubscriptionFailedException;

    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Position2dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
