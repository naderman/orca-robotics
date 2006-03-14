/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    @brief Odometry of 3D mobile robots.
    @{
*/

// define messages first

//! Position3d data structure
class Position3dData extends OrcaObject
{
    //! Robot pose in global CS according to odometry.
    Frame3d pose;
    //! Translational and angular velocities in the robot CS.
    //! This means that Vx is forward speed and Vy is side speed
    //! (possible only for some platforms).
    Twist motion;
    //! Are the motors stalled
    bool stalled;
};

//! Position3d geometry data structure
class Position3dGeometry extends OrcaObject
{
    //! Pose of the robot base, in the robot CS
    Frame3d offset;
    //! Dimensions of the base
    Size3d size;
};


/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 *
 */
interface Position3dConsumer
{
    void setData( Position3dData obj );
    
    // Do we need it here? Or if someone needs geometry they should pull it
    // with getGeometry(). In that case, do we rename this object Position3dDataConsumer?
    //void setGeometry( Position3dGeometry obj );
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

    // Returns the current configuration.
    //nonmutating Position3dConfig getConfig();
    
    //! Returns geometry of the position device.            
    nonmutating Position3dGeometry getGeometry();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @param preferedPushInterval The subscriber's preference for how often it wants to
     * receive updates [sec]. Provider's ability to fulfil this request may vary.
     *
     * @see unsubscribe
     */
    void subscribe( Position3dConsumer* subscriber )
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
    idempotent void unsubscribe( Position3dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
