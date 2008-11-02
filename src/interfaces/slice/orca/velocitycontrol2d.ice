/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_VELOCITY_CONTROL_2D_INTERFACE_ICE
#define ORCA2_VELOCITY_CONTROL_2D_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/vehicledescription.ice>

module orca
{
/*!
    @ingroup orca_interfaces
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
    @brief Sends velocity command to a 2D mobile robot
*/
interface VelocityControl2d
{
    //! Returns description of the vehicle
    idempotent VehicleDescription getDescription();

    //! Set desired velocity command
    idempotent void setCommand( VelocityControl2dData data )
            throws HardwareFailedException; 
};

//! @}
}; // module

#endif
