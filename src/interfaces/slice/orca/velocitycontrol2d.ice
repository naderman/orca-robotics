/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_VELOCITY_CONTROL_2D_INTERFACE_ICE
#define ORCA2_VELOCITY_CONTROL_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_velocitycontrol2d VelocityControl2d
    @brief Sends velocity command to a 2D mobile robot.
    @{
*/

//! Velocity command object
struct VelocityControl2dData
{
    //! Desired in-plane velocities
    Twist2d motion;
};

/*!
For simple descriptions and pictures of different mechanisms, see
http://www-128.ibm.com/developerworks/library/wi-robot5/index.html
*/
enum VelocityControl2dType
{
    //! Mechanism type is not specified
    VelocityControl2dUnspecified,
    //! Steers like a tank by individually controlling rotational velocities of left and right wheels (or tracks). 
    //! E.g. Pioneer-2, Segway RMP.
    VelocityControl2dDifferential,
    //! Steering is done with a single wheel. A tricycle works the same way.
    VelocityControl2dBycicle,
    //! Steering is done by synchronized motion of all wheels
    VelocityControl2dSynchro,
    //! Steering is done by two wheels, whose steering angle may be different.
    VelocityControl2dAckerman,
    //! Something not listed here
    VelocityControl2dOther
};

//! Static description. This description is put together at start time and does not change
//! during run time.
//!
//! @todo Provide a model of the drive mechanism from which the constraints can be deduced.
struct VelocityControl2dDescription
{
    //! Mechanism type. This may be important because some mechanisms impose certain constraints on 
    //! attainable velocities. For example, side speed (Vy) cannot be non-zero for most mechanisms, synchro drive
    //! being an exception.
    VelocityControl2dType type;

    //! Maximum values for in-plane velocities. 
    Twist2d maxVelocities;
};

/*!
    @brief Sends velocity command to a 2D mobile robot
*/
interface VelocityControl2d
{
    //! Returns description of the motion device.
    nonmutating VelocityControl2dDescription getDescription();

    //! Set desired velocity command
    idempotent void setCommand( VelocityControl2dData data )
            throws HardwareFailedException; 
};

//! @}
}; // module

#endif
