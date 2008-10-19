#ifndef GOALPLANNER_PROGRESSMONITOR_H
#define GOALPLANNER_PROGRESSMONITOR_H

#include <orca/pathfollower2d.h>
#include <IceUtil/IceUtil.h>
#include <orcaice/context.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>

namespace gbxiceutilacfr { class Thread; }

namespace goalplanner {

//
// @brief Keeps track of our current path and how far along we are.
//
// We have to recreate the functionality of orcaiface::ConsumerImpl here
// because PathFollower2dConsumer has non-standard methods.
//
// @author Alex Brooks
//
class ProgressMonitor : public IceUtil::Shared
{

public: 
    friend class PathFollower2dConsumerI;

    // constructor using interfaceTag (may throw ConfigFileException)
    ProgressMonitor( const orcaice::Context& context );
    ~ProgressMonitor();

    // copied from orcaiface::ConsumerImpl
    void subscribeWithTag( const std::string& interfaceTag, 
                          gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 );
    void subscribeWithString( const std::string& proxyString, 
                          gbxiceutilacfr::Thread*  thread, const std::string& subsysName="", 
                          int retryInterval=2, int retryNumber=-1 );
    void unsubscribeWithString( const std::string& proxyString );
    void unsubscribe();

    //
    // Functions called locally
    //

    // If we're currently following a path (ie both activated and enabled), 
    // returns true and sets currentWp to the waypoint we're currently seeking.
    // Otherwise, returns false.
    bool getCurrentWp( orca::Waypoint2d &currentWp );
    // return val same as getCurrentWp
    bool getCurrentPath( orca::PathFollower2dData &pathData, int &wpIndex );

private: 

    void internalSetWaypointIndex( int index );
    void internalSetActivationTime( const orca::Time &absoluteTime, double relativeTime );
    void internalSetData( const orca::PathFollower2dData &data );
    void internalSetEnabledState( bool enabledState );

    bool followingPath() const;

    bool gotData_;
    orca::PathFollower2dData pathData_;
    bool isEnabled_;
    int wpIndex_;

    IceUtil::Mutex mutex_;

    // Proxy to the internal consumer interface implementation
    orca::PathFollower2dConsumerPrx consumerPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr consumerPtr_;

    // Store the proxy of the interface after we subscribed to it. This lets us unsubscribe before destroying.
    // This store is empty initially, contains the proxy string after the subscription and is emptied again after
    // unsubscription. 
    gbxiceutilacfr::Store<std::string> proxyString_;

    // Component context.
    orcaice::Context context_;
};

typedef IceUtil::Handle<ProgressMonitor> ProgressMonitorPtr;

}

#endif
