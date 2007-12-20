#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"
#include "particle2dImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class Particle2dI : virtual public ::orca::Particle2d
{
public:

    Particle2dI( Particle2dImpl &impl )
        : impl_(impl) {}

    // remote functions

    virtual ::orca::Particle2dData getData(const Ice::Current&) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::Particle2dConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::Particle2dConsumerPrx& consumer,
                 const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

private:
    Particle2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

Particle2dImpl::Particle2dImpl( const std::string       &interfaceTag, 
                                const orcaice::Context  &context  )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

Particle2dImpl::Particle2dImpl( const orcaice::Context  &context,
                                const std::string       &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

Particle2dImpl::~Particle2dImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
Particle2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::Particle2dConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new Particle2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
Particle2dImpl::initInterface( hydroutil::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new Particle2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::Particle2dData
Particle2dImpl::internalGetData() const
{
    context_.tracer().debug( "Particle2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::Particle2dData data;
    dataStore_.get( data );
    return data;
}

void
Particle2dImpl::internalSubscribe(const ::orca::Particle2dConsumerPrx& subscriber)
{   
    context_.tracer().debug( "Particle2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"Particle2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
Particle2dImpl::internalUnsubscribe(const ::orca::Particle2dConsumerPrx& subscriber)
{
    context_.tracer().debug( "Particle2dImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
Particle2dImpl::localSetAndSend( const orca::Particle2dData& data )
{
    dataStore_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::Particle2dConsumerPrx,orca::Particle2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

}

