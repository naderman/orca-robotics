#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "util.h"
#include "qgraphics2dImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class QGraphics2dI : virtual public ::orca::QGraphics2d
{
public:

    QGraphics2dI( QGraphics2dImpl &impl )
        : impl_(impl) {}

    // remote functions

    virtual ::orca::QGraphics2dData getData(const Ice::Current&) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::QGraphics2dConsumerPrx& consumer,
                           const Ice::Current&)
        { impl_.internalSubscribe( consumer ); }

    virtual void unsubscribe(const ::orca::QGraphics2dConsumerPrx& consumer,
                 const Ice::Current&)
        { impl_.internalUnsubscribe( consumer ); }

private:
    QGraphics2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

QGraphics2dImpl::QGraphics2dImpl( const std::string       &interfaceTag, 
                                  const orcaice::Context  &context  )
    : interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

QGraphics2dImpl::QGraphics2dImpl( const orcaice::Context  &context,
                                  const std::string       &interfaceName )
    : interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

QGraphics2dImpl::~QGraphics2dImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
QGraphics2dImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::QGraphics2dConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new QGraphics2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
QGraphics2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new QGraphics2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::QGraphics2dData
QGraphics2dImpl::internalGetData() const
{
    context_.tracer().debug( "QGraphics2dImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::QGraphics2dData data;
    dataStore_.get( data );
    return data;
}

void
QGraphics2dImpl::internalSubscribe(const ::orca::QGraphics2dConsumerPrx& subscriber)
{   
    context_.tracer().debug( "QGraphics2dImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"QGraphics2dImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
QGraphics2dImpl::internalUnsubscribe(const ::orca::QGraphics2dConsumerPrx& subscriber)
{
    context_.tracer().debug( "QGraphics2dImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
QGraphics2dImpl::localSetAndSend( const orca::QGraphics2dData& data )
{
    dataStore_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::QGraphics2dConsumerPrx,orca::QGraphics2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

}

