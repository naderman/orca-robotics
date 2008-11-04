#include "progressmonitor.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

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


    virtual void setWaypointIndex( int index, const ::Ice::Current& )
        { return impl_.internalSetWaypointIndex( index ); }
    virtual void setActivationTime( const orca::Time &absoluteTime, double relativeTime, const ::Ice::Current& )
        { return impl_.internalSetActivationTime( absoluteTime, relativeTime ); }
    virtual void setData( const orca::PathFollower2dData &data, const ::Ice::Current& )
        { return impl_.internalSetData( data ); }
    virtual void setEnabledState( bool enabledState, const ::Ice::Current& )
        { return impl_.internalSetEnabledState( enabledState ); }

private:
    ProgressMonitor &impl_;
};

//////////////////////////////////////////////////////////////////////

ProgressMonitor::ProgressMonitor( const orcaice::Context& context ) :
    gotData_(false),
    isEnabled_(true),
    wpIndex_(-1),
    context_(context)
{
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
                        gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                        int retryInterval, int retryNumber )
{
    // this may throw ConfigFileException, we don't catch it, let the user catch it at the component level
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag );

    // now that we have the stingified proxy, use the function above.
    subscribeWithString( proxyString, thread, subsysName, retryInterval, retryNumber );
}

void 
ProgressMonitor::subscribeWithString( const std::string& proxyString, 
                        gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                        int retryInterval, int retryNumber )
{
    orca::PathFollower2dPrx providerPrx;
    // multi-try
    orcaice::connectToInterfaceWithString( context_,
                                            providerPrx,
                                            proxyString,
                                            thread,
                                            subsysName,
                                            retryInterval,
                                            retryNumber );

    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            providerPrx->subscribe( consumerPrx_ );
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

    proxyString_.set( proxyString );
}

void 
ProgressMonitor::unsubscribeWithString( const std::string& proxyString )
{
    orca::PathFollower2dPrx providerPrx;
    orcaice::connectToInterfaceWithString( context_, providerPrx, proxyString );

    providerPrx->unsubscribe( consumerPrx_ );
    std::stringstream ss;
    ss << "Unsubscribed from " << proxyString;
    context_.tracer().debug( ss.str() );

    proxyString_.purge();
}

void 
ProgressMonitor::unsubscribe()
{
    if ( proxyString_.isEmpty() )
        return;

    std::string proxyString;
    proxyString_.get( proxyString );
    unsubscribeWithString( proxyString );
}

void 
ProgressMonitor::internalSetWaypointIndex( int index )
{
    IceUtil::Mutex::Lock lock(mutex_);
    wpIndex_ = index;
}

void 
ProgressMonitor::internalSetActivationTime( const orca::Time &absoluteTime,
                                    double relativeTime )
{
    // IceUtil::Mutex::Lock lock(mutex_);
    // Nothing to do here.
}

void 
ProgressMonitor::internalSetData( const orca::PathFollower2dData &data )
{
    IceUtil::Mutex::Lock lock(mutex_);
    pathData_ = data;
    gotData_ = true;
}

void 
ProgressMonitor::internalSetEnabledState( bool enabledState )
{
    IceUtil::Mutex::Lock lock(mutex_);
    isEnabled_ = enabledState;
}

bool
ProgressMonitor::followingPath() const
{
    if ( !gotData_ ) return false;
    if ( !isEnabled_ ) return false;
    if ( wpIndex_ == -1 ) return false;
    if ( pathData_.path.size() == 0 ) return false;
    if ( wpIndex_ < 0 || wpIndex_ >= (int)(pathData_.path.size()) )
    {
        stringstream ss;
        ss << "Bad wpIndex_: " << wpIndex_ << " for path: " << orcaobj::toString(pathData_);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }    

    return true;
}

bool
ProgressMonitor::getCurrentWp( orca::Waypoint2d &currentWp )
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !followingPath() ) return false;

    currentWp = pathData_.path[wpIndex_];
    return true;
}

bool
ProgressMonitor::getCurrentPath( orca::PathFollower2dData &pathData, int &wpIndex )
{
    IceUtil::Mutex::Lock lock(mutex_);
    if ( !followingPath() ) return false;

    pathData = pathData_;
    wpIndex = wpIndex_;
    return true;
}

}

