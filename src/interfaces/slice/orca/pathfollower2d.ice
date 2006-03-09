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

#ifndef ORCA2_PATH_FOLLOWER_INTERFACE_ICE
#define ORCA2_PATH_FOLLOWER_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_pathfollower2d PathFollower2d
    @brief Interface for a component that can follow a 2d path, defined as a set of waypoints

The Pathfollower2d interface.

    @{
*/

//! A single waypoint
struct Waypoint2d {

    //! The pose of the waypoint
    Frame2d target;

    //! How close the follower has to get to the target
    float   distanceTolerance;

    //! How closely the follower has to match the target orientation
    float   headingTolerance;

    //! The time at which the follower should reach the waypoint, as
    //! an offset from the time the path was activated.
    Time    timeTarget;

    //! Max speed at which the follower can approach this waypoint
    float   maxApproachSpeed;

    //! Max turnrate the follower can use when approaching this waypoint
    float   maxApproachTurnrate;
};

//! A sequence of waypoints
sequence<Waypoint2d>        Path2d;

/*!
    WaypointFollower data structure
*/
class PathFollower2dData extends OrcaObject
{
    //! The path
    Path2d              path;
};

/*!
    @brief Consumer of path updates
*/
interface PathFollower2dConsumer
{
    //! Used by the follower to broadcast changes without transmitting the entire path.
    //!   Given that the follower is loaded up with a list of n waypoints,
    //!   the follower will call 'currentlySeekingWaypoint' with the index
    //!   of the waypoint it is _actively_ seeking.
    //!   - index is in the range [0,n-1]
    //!   - If the follower is loaded with a path but is inactive, it will call
    //      'currentlySeekingWaypoint(0)' on activation.
    idempotent void currentlySeekingWaypoint( int index );

    //! When the follower is loaded with a new path, it transmits that path to
    //! all subscribers.
    void setData( PathFollower2dData data );
};

/*!
    @brief The Path Follower

    To make the follower follow a path:

    - either (a)
      1) call 'setData(thePath,false)' to load the path, then
      2) call 'activateNow' to make it go.

    - or (b)
      1) call 'setData(thePath,true)' to load the path and have the
         follower start following it immediately
    
 */
interface PathFollower2d
{
    //! Returns the path currently being followed
    nonmutating PathFollower2dData      getData();
            
    //! Loads the follower with a path to follow.
    //! If 'activateImmediately' is false, the follower won't go anywhere
    //! until 'activateNow()' is called.
    //!
    //! Loading the follower with a new path while it is active
    //! over-writes the old path.  The activation status of the follower is
    //! set to the value of 'activateImmediately'.
    //!
    //! The follower can therefore be stopped by calling setData(dummyPath,false).
    //!
    idempotent void setData( PathFollower2dData path, bool activateImmediately );

    //! Start following the previously-loaded path.
    //! Throws DataNotExistException if a path wasn't previously loaded.
    idempotent void activateNow() throws DataNotExistException;

    //! Returns the state of the follower:
    //!   - true if it's actively following a path, or
    //!   - false if either:
    //!     (a) No path is loaded, or
    //!     (b) A path has been loaded but the follower hasn't been activated.
    nonmutating bool isActive();
    
    /*!
     *
     * Mimics IceStorm's subscribe().  The implementation may choose to
     * implement the push directly or use IceStorm.  This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     *
     */
    void subscribe( PathFollower2dConsumer *subscriber )
            throws SubscriptionFailedException;

    /**
     *
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     *
    **/
    idempotent void unsubscribe( PathFollower2dConsumer *subscriber );
};

/*! @} */
}; // module

#endif
