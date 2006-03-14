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

#ifndef ORCA2_PATHPLANNER_2D_INTERFACE_ICE
#define ORCA2_PATHPLANNER_2D_INTERFACE_ICE

#include <orca/bros1.ice>
#include <orca/pathfollower2d.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_pathplanner2d PathPlanner2d
    @brief PathPlanner interface for 2D world
    @{
*/


//! Error codes for results of planning a path
enum PathPlanner2dResult 
{ 
    PathOk, 
    PathStartOutsideMap, 
    PathDestinationOutsideMap, 
    PathDestinationUnreachable
};

//! Data structure for resulting path including error codes
class PathPlanner2dData extends OrcaObject
{
    Path2d path;
    PathPlanner2dResult result;    
};

/*!
    @brief Consumer of a planned path
*/
interface PathPlanner2dConsumer
{
    void setData( PathPlanner2dData obj );
};

//! 
//! Data structure holding a path planning task in
//! form of a starting poing, a "rough" path and a 
//! proxy that receives the computed path.
//!
class PathPlanner2dTask extends OrcaObject
{
    Waypoint2d start;
    Path2d coarsePath;
    PathPlanner2dConsumer prx;
};

/*!
    @brief Planning a path in 2D

    PathPlanner is an interface that accepts a task consisting of a start waypoing and a "rough" path. It serves a finer-grained path to the consumer via proxy. "setTask" returns the number of remaining tasks in the buffer.
*/
interface PathPlanner2d
{
    //! Set a task
    int setTask( PathPlanner2dTask task )
            throws BusyException, RequiredInterfaceFailedException;   
};

//! @}
}; // module

#endif
