/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PATH_FOLLOWER_INTERFACE_ICE
#define ORCA2_PATH_FOLLOWER_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
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
    ["slice2orca:string:degrees"] float   headingTolerance;

    //! The time at which the follower should reach the waypoint (in seconds), as
    //! an offset from the time the path was activated.
    double timeTarget;

    //! Max speed at which the follower can approach this waypoint
    float   maxApproachSpeed;

    //! Max turnrate the follower can use when approaching this waypoint
    ["slice2orca:string:degrees"] float   maxApproachTurnrate;
};

//! A sequence of waypoints
sequence<Waypoint2d>        Path2d;

/*!
    WaypointFollower data structure
*/
struct PathFollower2dData
{
    //! Time when data was measured.
    Time timeStamp;

    //! The path
    Path2d              path;
};

//! An enum to describe whether or not we have an active path
enum PathActivationEnum {
    FinishedPath,
    NoPathLoaded,
    PathLoadedButNotActivated,
    FollowingPath
};

struct PathFollower2dState
{
    //! Tells whether or not we're following a path
    PathActivationEnum pathActivation;

    //! A disabled PathFollower2d has it's "hands off the wheel"
    bool isEnabled;

    //! Only meaningful if pathActivation is 'FollowingPath'.
    //! Used to inform of progress without transmitting the entire path.
    //!   Given that the follower is loaded up with a list of n waypoints,
    //!   the follower will call 'setWaypointIndex' with the index
    //!   of the waypoint it is _actively_ seeking.
    //!   - waypoint indices are in the range [0,n-1]
    //!   - Suppose the follower has been loaded with a path but is inactive.  
    //!     On activation, the waypointIndex will become '0'.
    int waypointIndex;
    //! Only meaningful if pathActivation is 'FollowingPath'.
    //! The number of seconds we've been following the path for.
    double secondsSinceActivation;
};

/*!
    @brief Consumer of path updates
*/
interface PathFollower2dConsumer
{
    //! When the follower is loaded with a new path, it transmits that path to
    //! all subscribers.
    idempotent void setData( PathFollower2dData data );
    
    //! used by the PathFollower2d to broadcast its state to interested subscribers
    idempotent void setState( PathFollower2dState state );
};

//! Raised when we're activated but no path is loaded
exception PathNotLoadedException extends OrcaException {};

/*!
    @brief The Path Follower

    To make the follower follow a path:

    - either (a)
        -# call setData(thePath,false) to load the path, then
        -# call activateNow() to make it go.

    - or (b)
        -# call setData(thePath,true) to load the path and have the
         follower start following it immediately
 */
interface PathFollower2d
{
    //! Returns the path currently being followed, or throws DataNotExistException
    //! if none has been set.
    idempotent PathFollower2dData getData();
            
    //! Loads the follower with a path to follow.
    //! If @c activateImmediately is false, the follower won't go anywhere
    //! until activateNow() is called.
    //!
    //! Loading the follower with a new path while it is active
    //! over-writes the old path.  The activation status of the follower is
    //! set to the value of @c activateImmediately.
    //!
    //! The follower can therefore be stopped by calling setData(emptyPath,true).
    //!
    idempotent void setData( PathFollower2dData path, bool activateImmediately )
        throws MalformedParametersException, OrcaException;

    //! Reads the pathFollower2d's state
    idempotent PathFollower2dState getState();

    //! Start following the previously-loaded path.
    idempotent void activateNow()
        throws orca::PathNotLoadedException;

    //! Disabling the PathFollower stops it from sending any commands to the 
    //! robot.  It says, "Take your hands off the wheel!".  No commands can
    //! be send until it is re-enabled.
    idempotent void setEnabled( bool enabled );

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( PathFollower2dConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

/*! @} */
}; // module

#endif
