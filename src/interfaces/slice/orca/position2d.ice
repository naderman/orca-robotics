/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_POSITION_2D_INTERFACE_ICE
#define ORCA2_POSITION_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_position2d Position2d
    @brief Access to odometry of 2d mobile robitic bases.
    @{
*/

// define messages first
class Position2dData extends OrcaObject
{
    // Robot position
    Frame2d pose;
    // Translational and angular velocities in the robot CS
    Twist2d motion;
    // Are the motors stalled
    bool stalled;
};

class Position2dGeometry extends OrcaObject
{
    // Pose of the robot base, in the robot CS
    Frame2d offset;
    // Dimensions of the base
    Size2d size;
};

/*!
    @brief Access to odometry of 2d mobile robitic bases.
*/
interface Position2d
{
    // ClientPull_Supplier interface
    nonmutating Position2dData getData();
    nonmutating Position2dGeometry getGeometry();

    //idempotent void setPushRate(  );
};

//! Data consumer interface (needed only for the push pattern)
interface Position2dConsumer
{
    // ClientPush_Consumer interface
    void consumeData( Position2dData obj );
};


/!  @}
}; // module

#endif
