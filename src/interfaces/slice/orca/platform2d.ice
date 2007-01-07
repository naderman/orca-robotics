/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PLATFORM_2D_INTERFACE_ICE
#define ORCA2_PLATFORM_2D_INTERFACE_ICE

#include <orca/bros1.ice>
#include <orca/position2d.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_platform2d Platform2d
    @brief Active robotic platform
    @{
*/

//! Velocity command object
struct Velocity2dCommand
{
    //! Time when data was measured.
    Time timeStamp;

    //! Commanded in-plane velocities.
    Twist2d motion;
};

//! Platform configuration object
struct Platform2dConfig
{
    //! Time when data was measured.
    Time timeStamp;
    //! Maximum values for in-plane velocities. 
    Twist2d maxVelocities;
};

/*!
    @brief Control mobile robot bases in 2D

    Platform2d is an "active" Position2d. In addition to serving its current position2d it also accepts motion commands.

    Note that this platform only accepts velocity commands (unlike in Player where position2d interface also accepts waypoint commands). The rationale is that planners/navigators should think in terms of waypoints, not the hardware.
*/
interface Platform2d extends Position2d
{
    //! Get configuration
    nonmutating Platform2dConfig getConfig();

    //! Set velocity command
    idempotent void setCommand( Velocity2dCommand data )
            throws HardwareFailedException;

    //! Set configuration
    idempotent void setConfig( Platform2dConfig config )
            throws ConfigurationNotExistException;

    //! Enable motor(s)
    idempotent void enableMotor( bool enable )
            throws HardwareFailedException;    
};

//! @}
}; // module

#endif
