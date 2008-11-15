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

    EStopI( EStopImpl             &impl,
            AbstractEStopCallback &callback )
        : impl_(impl),
          callback_(callback) {}
    virtual ::orca::EStopData getData(const Ice::Current&)
        { return impl_.internalGetData(); }
    virtual IceStorm::TopicPrx subscribe(const orca::EStopConsumerPrx& subscriber, const ::Ice::Current& = ::Ice::Current())
        { return impl_.internalSubscribe(subscriber); }

    virtual void activateEStop( const Ice::Current& )
        { callback_.activateEStop(); }
    virtual void keepAlive( const Ice::Current& )
        { callback_.keepAlive(); }
    virtual double getRequiredKeepAlivePeriodSec( const Ice::Current& )
        { return callback_.getRequiredKeepAlivePeriodSec(); }
    virtual void setToOperatingMode( const Ice::Current& )
        { callback_.setToOperatingMode(); }

private:
    EStopImpl            &impl_;
    AbstractEStopCallback &callback_;
};

//////////////////////////////////////////////////////////////////////

EStopImpl::EStopImpl( AbstractEStopCallback  &callback,
                      const std::string       &interfaceTag, 
                      const orcaice::Context  &context  ) : 
    callback_(callback),
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

EStopImpl::EStopImpl( AbstractEStopCallback  &callback,
                      const orcaice::Context  &context,
                      const std::string       &interfaceName ) : 
    callback_(callback),
    interfaceName_(interfaceName),
    context_(context)
{
    init();
}

EStopImpl::~EStopImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
EStopImpl::init()
{
    ptr_ = new EStopI( *this, callback_ );

    topicHandler_.reset( new EStopTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );
}

void
EStopImpl::initInterface()
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    topicHandler_->connectToTopic();
}

void 
EStopImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );

    topicHandler_->connectToTopic( thread, subsysName, retryInterval );
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

IceStorm::TopicPrx 
EStopImpl::internalSubscribe(const orca::EStopConsumerPrx& subscriber)
{
    if ( !topicHandler_.get() ) 
    {
        throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
    }
    
    // if we have data, send all the information we have to the new subscriber (and to no one else)
    if ( dataStore_.isEmpty() )
    {
        return topicHandler_->subscribe( subscriber );
    }
    else
    {
        orca::EStopData data;
        dataStore_.get( data );
    
        return topicHandler_->subscribe( subscriber, data );
    }
}

void
EStopImpl::localSetAndSend( const orca::EStopData& data )
{
    dataStore_.set( data );
    
    topicHandler_->publish( data );
}

}

