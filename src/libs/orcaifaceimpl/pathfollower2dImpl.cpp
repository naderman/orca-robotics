#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "util.h"
#include "pathfollower2dImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PathFollower2dI : virtual public ::orca::PathFollower2d
{
public:

    PathFollower2dI( PathFollower2dImpl   &impl,
                     PathFollowerCallback &callback )
        : impl_(impl),
          callback_(callback) {}

    // remote functions

    virtual ::orca::PathFollower2dData getData(const Ice::Current&) const
        { return impl_.internalGetData(); }
    virtual void subscribe(const ::orca::PathFollower2dConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }
    virtual void unsubscribe(const ::orca::PathFollower2dConsumerPrx& consumer,
                 const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

    virtual void setData( const orca::PathFollower2dData &path, bool activateImmediately, const Ice::Current& )
        { callback_.setData(path,activateImmediately); }
    virtual void activateNow(const Ice::Current&)
        { callback_.activateNow(); }
    virtual int  getWaypointIndex(const Ice::Current&) const
        { return callback_.getWaypointIndex(); }
    virtual bool getAbsoluteActivationTime( orca::Time &activationTime, const Ice::Current& ) const
        { return callback_.getAbsoluteActivationTime(activationTime); }
    virtual bool getRelativeActivationTime( double &secondsSinceActivation, const Ice::Current& ) const
        { return callback_.getRelativeActivationTime(secondsSinceActivation); }
    virtual void setEnabled( bool enabled, const Ice::Current& )
        { callback_.setEnabled(enabled); }
    virtual bool enabled(const Ice::Current&) const
        { return callback_.enabled(); }

private:
    PathFollower2dImpl   &impl_;
    PathFollowerCallback &callback_;
};

//////////////////////////////////////////////////////////////////////

PathFollower2dImpl::PathFollower2dImpl( PathFollowerCallback    &callback,
                                        const std::string       &interfaceTag, 
                                        const orcaice::Context  &context  )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      callback_(callback),
      context_(context)
{
}

PathFollower2dImpl::PathFollower2dImpl( PathFollowerCallback    &callback,
                                        const orcaice::Context  &context,
                                        const std::string       &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      callback_(callback),
      context_(context)
{
}

PathFollower2dImpl::~PathFollower2dImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
PathFollower2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::PathFollower2dConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PathFollower2dI( *this, callback_ );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PathFollower2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new PathFollower2dI( *this, callback_ );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::PathFollower2dData
PathFollower2dImpl::internalGetData() const
{
    context_.tracer().debug( "PathFollower2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PathFollower2dData data;
    dataStore_.get( data );
    return data;
}

void
PathFollower2dImpl::internalSubscribe(const ::orca::PathFollower2dConsumerPrx& subscriber)
{   
    context_.tracer().debug( "PathFollower2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"PathFollower2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
PathFollower2dImpl::internalUnsubscribe(const ::orca::PathFollower2dConsumerPrx& subscriber)
{
    context_.tracer().debug( "PathFollower2dImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
PathFollower2dImpl::localSetAndSend( const orca::PathFollower2dData& data )
{
    dataStore_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::PathFollower2dConsumerPrx,orca::PathFollower2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

void 
PathFollower2dImpl::localSetWaypointIndex( int index )
{
    // check that communicator still exists
    if ( !context_.communicator() ) {
        return;
    }

    try {
        consumerPrx_->setWaypointIndex(index);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = detail::tryReconnectToIceStorm( context_,
                                                           consumerPrx_,
                                                           topicPrx_,
                                                           interfaceName_,
                                                           topicName_ );
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                consumerPrx_->setWaypointIndex(index);
            }
            catch ( Ice::Exception &e )
            {
                std::stringstream ss;
                ss << interfaceName_ << ": Re-push of data failed: " << e;
                context_.tracer().info( ss.str() );
            }
        }
    }
}
void 
PathFollower2dImpl::localSetActivationTime( orca::Time absoluteTime, double relativeTime )
{
    // check that communicator still exists
    if ( !context_.communicator() ) {
        return;
    }

    try {
        consumerPrx_->setActivationTime(absoluteTime,relativeTime);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = detail::tryReconnectToIceStorm( context_,
                                                           consumerPrx_,
                                                           topicPrx_,
                                                           interfaceName_,
                                                           topicName_ );
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                consumerPrx_->setActivationTime(absoluteTime,relativeTime);
            }
            catch ( Ice::Exception &e )
            {
                std::stringstream ss;
                ss << interfaceName_ << ": Re-push of data failed: " << e;
                context_.tracer().info( ss.str() );
            }
        }
    }
}
void 
PathFollower2dImpl::localSetEnabledState( bool enabledState )
{
    // check that communicator still exists
    if ( !context_.communicator() ) {
        return;
    }

    try {
        consumerPrx_->setEnabledState(enabledState);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = detail::tryReconnectToIceStorm( context_,
                                                           consumerPrx_,
                                                           topicPrx_,
                                                           interfaceName_,
                                                           topicName_ );
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                consumerPrx_->setEnabledState(enabledState);
            }
            catch ( Ice::Exception &e )
            {
                std::stringstream ss;
                ss << interfaceName_ << ": Re-push of data failed: " << e;
                context_.tracer().info( ss.str() );
            }
        }
    }
}

}

