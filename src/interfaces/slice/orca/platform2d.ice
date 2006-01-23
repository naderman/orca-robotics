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
class Velocity2dCommand extends OrcaObject
{
    Twist2d motion;
};

//! Platform configuration object
class Platform2dConfig extends OrcaObject
{
    Twist2d maxMotionCommand;
};

/*!
    @brief Control mobile robot bases in 2D

    Platform2d is an "active" Position2d. In addition to serving its current position2d it also accepts motion commands. Note that this platform only accepts velocity commands (unlike in Player where position2d interface also accepts waypoint commands). The rational is that planners/navigators should think in terms of waypoints, not the hardware.
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
