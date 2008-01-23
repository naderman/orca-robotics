#ifndef GOALPLANNER_PROGRESSMONITOR_H
#define GOALPLANNER_PROGRESSMONITOR_H

#include <orca/pathfollower2d.h>
#include <IceUtil/IceUtil.h>

namespace goalplanner {

//
// @brief Keeps track of our current path and how far along we are.
//
// @author Alex Brooks
//
class ProgressMonitor : public orca::PathFollower2dConsumer
{

public: 

    ProgressMonitor();

    //
    // Functions called locally
    //

    // If we're currently following a path (ie both activated and enabled), 
    // returns true and sets currentWp to the waypoint we're currently seeking.
    // Otherwise, returns false.
    bool getCurrentWp( orca::Waypoint2d &currentWp );
    // return val same as getCurrentWp
    bool getCurrentPath( orca::PathFollower2dData &pathData, int &wpIndex );

    //
    // Functions called remotely
    //
    void setWaypointIndex( int index, const ::Ice::Current& =::Ice::Current() );
    void setActivationTime( const orca::Time &absoluteTime, double relativeTime, const ::Ice::Current& =::Ice::Current() );
    void setData( const orca::PathFollower2dData &data, const ::Ice::Current& =::Ice::Current() );
    void setEnabledState( bool enabledState, const ::Ice::Current& =::Ice::Current() );

private: 

    bool followingPath() const;

    bool gotData_;
    orca::PathFollower2dData pathData_;
    bool isEnabled_;
    int wpIndex_;

    IceUtil::Mutex mutex_;

};

}

#endif
