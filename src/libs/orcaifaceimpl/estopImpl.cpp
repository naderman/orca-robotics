#include <iostream>
#include <orcaice/orcaice.h>
 
#include "estopImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class EStopI : virtual public ::orca::EStop
{
public:

    EStopI( EStopImpl            &impl,
            EStopNonStandardImpl &nonStandardImpl )
        : impl_(impl),
          nonStandardImpl_(nonStandardImpl) {}

    // remote functions

    virtual ::orca::EStopData getData(const Ice::Current&)
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::EStopConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::EStopConsumerPrx& consumer,
                             const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

    virtual void activateEStop( const Ice::Current& )
        { nonStandardImpl_.activateEStop(); }
    virtual void keepAlive( const Ice::Current& )
        { nonStandardImpl_.keepAlive(); }
    virtual double getRequiredKeepAlivePeriodSec( const Ice::Current& )
        { return nonStandardImpl_.getRequiredKeepAlivePeriodSec(); }
    virtual void setToOperatingMode( const Ice::Current& )
        { nonStandardImpl_.setToOperatingMode(); }

private:
    EStopImpl            &impl_;
    EStopNonStandardImpl &nonStandardImpl_;
};

//////////////////////////////////////////////////////////////////////

EStopImpl::EStopImpl( EStopNonStandardImpl    &eStopNonStandardImpl,
                      const std::string       &interfaceTag, 
                      const orcaice::Context  &context  )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName_)),
      eStopNonStandardImpl_(eStopNonStandardImpl),
      context_(context)
{
}

EStopImpl::EStopImpl( EStopNonStandardImpl    &eStopNonStandardImpl,
                      const orcaice::Context  &context,
                      const std::string       &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(orcaice::getTopicNameFromInterfaceName(context,interfaceName)),
      eStopNonStandardImpl_(eStopNonStandardImpl),
      context_(context)
{
}

EStopImpl::~EStopImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
EStopImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::EStopConsumerPrx>
        ( context_, publisherPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new EStopI( *this, eStopNonStandardImpl_ );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
EStopImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisherPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new EStopI( *this, eStopNonStandardImpl_ );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::EStopData
EStopImpl::internalGetData() const
{
    context_.tracer().debug( "EStopImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::EStopData data;
    dataStore_.get( data );
    return data;
}

void
EStopImpl::internalSubscribe(const ::orca::EStopConsumerPrx& subscriber)
{   
    context_.tracer().debug( "EStopImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"EStopImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
EStopImpl::internalUnsubscribe(const ::orca::EStopConsumerPrx& subscriber)
{
    context_.tracer().debug( "EStopImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
EStopImpl::localSetAndSend( const orca::EStopData& data )
{
    dataStore_.set( data );
    
    // Try to push to IceStorm.
    orcaice::tryPushToIceStormWithReconnect<orca::EStopConsumerPrx,orca::EStopData>
        ( context_,
          publisherPrx_,
          data,
          topicPrx_,
          topicName_ );
}

}

