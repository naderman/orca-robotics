#include "progressmonitor.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/pathfollower2d.h>
#include <orcaobj/pathplanner2d.h>
#include <orcaifaceutil/pathfollower2d.h>

using namespace std;

namespace goalplanner {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PathFollower2dConsumerI : public orca::PathFollower2dConsumer
{
public:

    PathFollower2dConsumerI( ProgressMonitor &impl )
        : impl_(impl) {}
    virtual ~PathFollower2dConsumerI() {}

    // remote interface


    virtual void setState( const orca::PathFollower2dState &state, const ::Ice::Current& )
        { return impl_.internalSetState( state ); }
    virtual void setData( const orca::PathFollower2dData &data, const ::Ice::Current& )
        { return impl_.internalSetData( data ); }

private:
    ProgressMonitor &impl_;
};

//////////////////////////////////////////////////////////////////////

ProgressMonitor::ProgressMonitor( const orcaice::Context& context ) :
    gotData_(false),
    context_(context)
{
    state_.waypointIndex  = 0;
    state_.pathActivation = orca::NoPathLoaded;

    consumerPtr_ = new PathFollower2dConsumerI( *this );
    // this function does not throw, because it never talks to the Registry
    consumerPrx_ = orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>
        (context_,consumerPtr_);
}

ProgressMonitor::~ProgressMonitor()
{
    context_.tracer().debug( "ConsumerImpl::~ConsumerImpl()" );
    // unsubscribe from the info provider
    try {
        unsubscribe();
    }
    catch ( const std::exception& e ) {
        std::stringstream ss;
        ss << "failed to unsubscribe in destructor: " << e.what();
        context_.tracer().warning( ss.str() );
    }
    catch ( ... ) {
        context_.tracer().warning( "failed to unsubscribe in destructor." );
    }

    // now destroy our consumer object
    if ( !consumerPrx_ )
        return;

    orcaice::tryRemoveInterfaceWithIdentity( context_, consumerPrx_->ice_getIdentity() );
}

void 
ProgressMonitor::subscribeWithTag( const std::string& interfaceTag, 
                        gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
                        int retryInterval, int retryNumber )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    subscribeWithString( proxyString, activity, subsysName, retryInterval, retryNumber );
}

void 
ProgressMonitor::subscribeWithString( const std::string& proxyString, 
                        gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
                        int retryInterval, int retryNumber )
{
    orca::PathFollower2dPrx providerPrx;
    // multi-try
    orcaice::connectToInterfaceWithString( context_,
                                            providerPrx,
                                            proxyString,
                                            activity,
                                            subsysName,
                                            retryInterval,
                                            retryNumber );

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            IceStorm::TopicPrx topicPrx = providerPrx->subscribe( consumerPrx_ );
            topic_.set( topicPrx );

            std::stringstream ss;
            ss << "Subscribed to " << proxyString;
            context_.tracer().debug( ss.str() );
            break;
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "Failed to subscribe: " << e.what() << std::endl
                <<"Will retry in "<<retryInterval<<"s.";
            context_.tracer().warning( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "Failed to subscribe for unknown reason. "
                <<"Will retry in "<<retryInterval<<"s.";
            context_.tracer().warning( ss.str() );
        }
        ++count;
        if ( !subsysName.empty() )
            context_.status().heartbeat( subsysName );
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() )
            context_.status().heartbeat( subsysName );
    }
}

void 
ProgressMonitor::unsubscribe()
{
    if ( !topic_.isEmpty() )
    {
        IceStorm::TopicPrx topicPrx;
        topic_.get( topicPrx );

        topicPrx->unsubscribe( consumerPrx_ );
        std::stringstream ss;
        ss << "Unsubscribed from " << topicPrx->ice_toString();
        context_.tracer().debug( ss.str() );
    }
}

void 
ProgressMonitor::internalSetState( const orca::PathFollower2dState &state )
{
    IceUtil::Mutex::Lock lock(mutex_);
    state_ = state;
}

void 
ProgressMonitor::internalSetData( const orca::PathFollower2dData &data )
{
    IceUtil::Mutex::Lock lock(mutex_);
    gotData_ = true;
    pathData_ = data;
}

bool
ProgressMonitor::followingPath() const
{
    bool isFollowingPath = ( gotData_ &&
                             state_.isEnabled && 
                             state_.pathActivation == orca::FollowingPath );
    if ( isFollowingPath )
    {
        if ( state_.waypointIndex < 0 || state_.waypointIndex >= (int)(pathData_.path.size()) )
        {
            stringstream ss;
            ss << "Bad state_: " << ifaceutil::toString(state_) << " for path: " << orcaobj::toString(pathData_);
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }    
    }
    return isFollowingPath;
}

bool
ProgressMonitor::getCurrentWp( orca::Waypoint2d &currentWp )
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !followingPath() ) return false;

    currentWp = pathData_.path[state_.waypointIndex];
    return true;
}

bool
ProgressMonitor::getCurrentPath( orca::PathFollower2dData &pathData, int &wpIndex )
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !followingPath() ) return false;

    pathData = pathData_;
    wpIndex = state_.waypointIndex;
    return true;
}

}

