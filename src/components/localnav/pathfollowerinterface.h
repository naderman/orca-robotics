#ifndef LOCALNAV_PATHFOLLOWERINTERFACE_H
#define LOCALNAV_PATHFOLLOWERINTERFACE_H

#include <orcaifaceimpl/pathfollower2d.h>
#include "clock.h"

namespace localnav {

//
// @brief Implements the orca::PathFollower2d interface.
//        Basically we just wrap up an orcaifaceimpl version, but since this interface is special
//        we have to add a few things.
//
// @author Alex Brooks
//
class PathFollowerInterface : public orcaifaceimpl::AbstractPathFollowerCallback
{

public: 

    PathFollowerInterface( const Clock            &clock,
                           const std::string      &interfaceTag,
                           const orcaice::Context &context );

    void initInterface();
    void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 );

    // Allow localnav's internals to increment the waypoint index
    void updateActivationStateAndWaypointIndex( orca::PathActivationEnum activationState,
                                                int                      waypointIndex );
    // Allow component internals to check for pending requests.
    // Sets pathData on _either_ gotNewPath or gotPathActivation.
    void serviceRequests( bool                     &gotNewPath,
                          bool                     &gotPathActivation,
                          orca::PathFollower2dData &pathData,
                          orca::Time               &pathStartTime );

    // From orcaifaceimpl::PathFollowerCallback
    virtual void setData( const orca::PathFollower2dData &pathData, bool activateImmediately );
    virtual void activateNow();
    virtual orca::PathFollower2dState getState();
    virtual void setEnabled( bool enabled );
    virtual bool enabled();

private: 

    // Holds new requests from the outside world till we poll for them in another thread.
    // Thread-safe
    class NewlyArrivedRequestStore {
    public:
        NewlyArrivedRequestStore()
            : haveNewPath_(false),
              havePendingActivationRequest_(false)
            {}
        
        // Set functions
        void addPathRequest( const orca::PathFollower2dData &pathData, bool activateImmediately );
        void addActivationRequest();

        // Get function (we remember what's gotten, only giving it out once)
        void get( bool &newPathArrived,
                  bool &activationArrived,
                  orca::PathFollower2dData &pathData );

    private:
        // New paths from the outside world go in here
        std::auto_ptr<orca::PathFollower2dData> pendingPathRequest_;
        bool haveNewPath_;
        bool havePendingActivationRequest_;

        IceUtil::Mutex mutex_;
    };
    
    NewlyArrivedRequestStore newlyArrivedRequestStore_;

    orcaifaceimpl::PathFollower2dImplPtr pathFollower2dImpl_;

    orca::PathFollower2dState pathFollower2dState_;
    orca::Time activationTime_;
    const Clock &clock_;
    
    // Protects our data
    IceUtil::Mutex mutex_;

    orcaice::Context context_;
};

}

#endif
