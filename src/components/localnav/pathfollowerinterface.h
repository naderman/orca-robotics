#ifndef LOCALNAV_PATHFOLLOWERINTERFACE_H
#define LOCALNAV_PATHFOLLOWERINTERFACE_H

#include <orcaifaceimpl/pathfollower2dImpl.h>
#include "clock.h"

namespace localnav {

//!
//! @author Alex Brooks
//!
class PathFollowerInterface : public orcaifaceimpl::PathFollowerCallback
{

public: 

    PathFollowerInterface( const Clock &clock,
                           const std::string &interfaceTag,
                           const orcaice::Context &context );

    void initInterface() { pathFollower2dImpl_->initInterface(); }

    // Allow localnav's internals to increment the waypoint index
    void updateWaypointIndex( int index );
    // Allow component internals to check for pending requests
    void serviceRequests( bool                     &gotNewPath,
                          orca::PathFollower2dData &pathData,
                          bool                     &gotActivation,
                          orca::Time               &pathStartTime );

    // From orcaifaceimpl::PathFollowerCallback
    void setData( const orca::PathFollower2dData &pathData, bool activateImmediately );
    void activateNow();
    int  getWaypointIndex() const;
    bool getAbsoluteActivationTime( orca::Time &activationTime ) const;
    bool getRelativeActivationTime( double &secondsSinceActivation ) const;
    void setEnabled( bool enabled );
    bool enabled() const;

private: 

    double timeSinceActivate( const orca::Time &activationTime );

    orcaifaceimpl::PathFollower2dImplPtr pathFollower2dImpl_;

    // New paths from the outside world go in here
    gbxsickacfr::gbxiceutilacfr::Store<orca::PathFollower2dData>        pendingPathRequestStore_;

    // Time of Activation from the outside world goes in here
    gbxsickacfr::gbxiceutilacfr::Store<orca::Time>                      pendingActivationRequestStore_;

    // Progress info from the component goes in here
    gbxsickacfr::gbxiceutilacfr::Store<int>                             wpIndexStore_;
    gbxsickacfr::gbxiceutilacfr::Store<orca::Time>                      activationTimeStore_;

    // Allow external en/dis-able
    gbxsickacfr::gbxiceutilacfr::Store<bool>                            enabledStore_;

    const Clock &clock_;

    orcaice::Context context_;
};

}

#endif
