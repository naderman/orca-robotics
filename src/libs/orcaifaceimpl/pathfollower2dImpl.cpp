#include <iostream>
#include <orcaice/orcaice.h>
 
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

    virtual ::orca::PathFollower2dData getData(const Ice::Current&)
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
    virtual int  getWaypointIndex(const Ice::Current&)
        { return callback_.getWaypointIndex(); }
    virtual bool getAbsoluteActivationTime( orca::Time &activationTime, const Ice::Current& )
        { return callback_.getAbsoluteActivationTime(activationTime); }
    virtual bool getRelativeActivationTime( double &secondsSinceActivation, const Ice::Current& )
        { return callback_.getRelativeActivationTime(secondsSinceActivation); }
    virtual void setEnabled( bool enabled, const Ice::Current& )
        { callback_.setEnabled(enabled); }
    virtual bool enabled(const Ice::Current&)
        { return callback_.enabled(); }

private:
    PathFollower2dImpl   &impl_;
    PathFollowerCallback &callback_;
};

//////////////////////////////////////////////////////////////////////

PathFollower2dImpl::PathFollower2dImpl( PathFollowerCallback    &callback,
                                        const std::string       &interfaceTag, 
                                        const orcaice::Context  &context  )
    : interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
      topicName_(orcaice::toTopicAsString(context.name(),interfaceName_)),
      callback_(callback),
      context_(context)
{
}

PathFollower2dImpl::PathFollower2dImpl( PathFollowerCallback    &callback,
                                        const orcaice::Context  &context,
                                        const std::string       &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(orcaice::toTopicAsString(context.name(),interfaceName_)),
      callback_(callback),
      context_(context)
{
}

PathFollower2dImpl::~PathFollower2dImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
PathFollower2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::PathFollower2dConsumerPrx>
        ( context_, publisherPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new PathFollower2dI( *this, callback_ );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PathFollower2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

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
    orcaice::tryPushToIceStormWithReconnect<orca::PathFollower2dConsumerPrx,orca::PathFollower2dData>
        ( context_,
          publisherPrx_,
          data,
          topicPrx_,
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
        publisherPrx_->setWaypointIndex(index);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = orcaice::detail::tryReconnectToIceStorm( context_,
                                                           publisherPrx_,
                                                           topicPrx_,
                                                           topicName_ );
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                publisherPrx_->setWaypointIndex(index);
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
        publisherPrx_->setActivationTime(absoluteTime,relativeTime);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = orcaice::detail::tryReconnectToIceStorm( context_,
                                                           publisherPrx_,
                                                           topicPrx_,
                                                           topicName_ );
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                publisherPrx_->setActivationTime(absoluteTime,relativeTime);
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
        publisherPrx_->setEnabledState(enabledState);
    }
    catch ( Ice::CommunicatorDestroyedException & )
    {
        // If we see this, we're obviously shutting down.  Don't bitch about anything.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss; ss << interfaceName_ << "::" << __func__ << ": Failed push to IceStorm: " << e;
        context_.tracer().warning( ss.str() );

        bool reconnected = orcaice::detail::tryReconnectToIceStorm( context_,
                                                           publisherPrx_,
                                                           topicPrx_,
                                                           topicName_ );
        if ( reconnected )
        {
            try {
                // try again to push that bit of info
                publisherPrx_->setEnabledState(enabledState);
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

