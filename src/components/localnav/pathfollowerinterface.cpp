#include "pathfollowerinterface.h"
#include <iostream>
#include <cmath>
#include <orcaobj/pathfollower2d.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace localnav {

////////////////////////////////////////////////////////////////////////////////

PathFollowerInterface::PathFollowerInterface( const Clock &clock,
                                              const std::string &interfaceTag,
                                              const orcaice::Context &context )
    : pathFollower2dImpl_(new orcaifaceimpl::PathFollower2dImpl(*this,interfaceTag,context)),
      clock_(clock),
      context_(context)
{
    // We're inactive but enabled on initialization
    pathFollower2dState_.pathActivation = orca::NoPathLoaded;
    pathFollower2dState_.isEnabled      = true;
}

void
PathFollowerInterface::initInterface()
{ 
    pathFollower2dImpl_->initInterface(); 
}

void
PathFollowerInterface::initInterface( gbxutilacfr::Stoppable* activity,
                                      const std::string&      subsysName,
                                      int                     retryInterval )
{
    pathFollower2dImpl_->initInterface( activity, subsysName, retryInterval );
}

void
PathFollowerInterface::setData( const orca::PathFollower2dData &pathData, bool activateImmediately )
{
    stringstream ss;
    ss<<"pathfollower2dI: Received new path: " << orcaobj::toVerboseString(pathData) << endl
      <<"activateImmediately: " << activateImmediately << endl;
    context_.tracer().debug( ss.str(), 2 );

    // Sanity check
    std::string reason;
    if ( !orcaobj::isSane( pathData, reason ) )
    {
        stringstream ss;
        ss << "PathFollower2dI: Received screwy path: " << orcaobj::toString(pathData) << endl << reason;
        context_.tracer().warning( ss.str() );
        throw orca::MalformedParametersException( reason );
    }

    if ( orcaobj::isPathSketchy( pathData.path, reason ) )
    {
        stringstream ss;
        ss << "Received dodgy path: " << orcaobj::toVerboseString(pathData) << endl << reason;
        context_.tracer().warning( ss.str() );
    }

    newlyArrivedRequestStore_.addPathRequest( pathData, activateImmediately );
}

void
PathFollowerInterface::activateNow()
{
    newlyArrivedRequestStore_.addActivationRequest();
}

orca::PathFollower2dState
PathFollowerInterface::getState()
{
    IceUtil::Mutex::Lock lock( mutex_ );
    if ( pathFollower2dState_.pathActivation == orca::FollowingPath )
    {
        pathFollower2dState_.secondsSinceActivation = orcaice::timeDiffAsDouble( clock_.time(),
                                                                                 activationTime_ );
    }
    return pathFollower2dState_;
}

void
PathFollowerInterface::setEnabled( bool enabled )
{
    orca::PathFollower2dState tempState;
    {
        IceUtil::Mutex::Lock lock( mutex_ );
        pathFollower2dState_.isEnabled = enabled;
        tempState = pathFollower2dState_;
    }

    // let the consumers know that the enabled state changed (outside crit section)
    pathFollower2dImpl_->localSetState( tempState );
}

bool
PathFollowerInterface::enabled()
{
    IceUtil::Mutex::Lock lock( mutex_ );
    return pathFollower2dState_.isEnabled;
}

void 
PathFollowerInterface::updateActivationStateAndWaypointIndex( orca::PathActivationEnum activationState,
                                                              int waypointIndex )
{
    orca::PathFollower2dState tempState;
    {
        IceUtil::Mutex::Lock lock( mutex_ );
        pathFollower2dState_.waypointIndex      = waypointIndex;
        pathFollower2dState_.pathActivation     = activationState;
        pathFollower2dState_.secondsSinceActivation = orcaice::timeDiffAsDouble( clock_.time(),
                                                                                 activationTime_ );
        tempState = pathFollower2dState_;
    }
    // Let the world know (outside crit section)
    pathFollower2dImpl_->localSetState( tempState );
}

void
PathFollowerInterface::serviceRequests( bool                     &gotNewPath,
                                        bool                     &gotPathActivation,
                                        orca::PathFollower2dData &pathData,
                                        orca::Time               &pathStartTime )
{
    bool newPathArrived, activationArrived;
    newlyArrivedRequestStore_.get( newPathArrived, activationArrived, pathData );
    if ( newPathArrived )
    {
        pathFollower2dImpl_->localSetAndSend( pathData );        
        gotNewPath = true;
    }
    if ( activationArrived )
    {
        orca::PathFollower2dState tempState;
        {
            IceUtil::Mutex::Lock lock( mutex_ );
            activationTime_ = clock_.time();
            pathFollower2dState_.pathActivation         = orca::FollowingPath;
            pathFollower2dState_.waypointIndex          = 0;
            pathFollower2dState_.secondsSinceActivation = 0.0;
            tempState = pathFollower2dState_;
        }
        gotPathActivation = true;
        pathStartTime = activationTime_;
        pathFollower2dImpl_->localSetState( tempState );
    }
}

//////////////////////////////////////////////////////////////////////

void
PathFollowerInterface::NewlyArrivedRequestStore::addPathRequest( const orca::PathFollower2dData &pathData,
                                                                 bool activateImmediately )
{
    IceUtil::Mutex::Lock lock( mutex_ );
    pendingPathRequest_.reset( new orca::PathFollower2dData(pathData) );
    haveNewPath_ = true;
    havePendingActivationRequest_ = activateImmediately;
}

void
PathFollowerInterface::NewlyArrivedRequestStore::addActivationRequest()
{
    IceUtil::Mutex::Lock lock( mutex_ );
    if ( !pendingPathRequest_.get() )
    {
        throw orca::PathNotLoadedException( "No path is loaded, activation is meaningless." );
    }
    havePendingActivationRequest_ = true;
}

void
PathFollowerInterface::NewlyArrivedRequestStore::get( bool &newPathArrived,
                                                      bool &activationArrived,
                                                      orca::PathFollower2dData &pathData )
{
    IceUtil::Mutex::Lock lock( mutex_ );
    if ( haveNewPath_ || havePendingActivationRequest_ )
    {
        assert( pendingPathRequest_.get() );
        pathData = *pendingPathRequest_;
    }
    newPathArrived    = haveNewPath_;
    activationArrived = havePendingActivationRequest_;

    if ( havePendingActivationRequest_ )
    {
        pendingPathRequest_.reset(0);
    }
    haveNewPath_                  = false;
    havePendingActivationRequest_ = false;
}


}

