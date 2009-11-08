#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaice/convertutils.h>
 
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

    virtual orca::EStopDescription getDescription(const Ice::Current&)
        { return impl_.getDescription(); }
    virtual void setEnabled( bool enabled, const Ice::Current& )
        { callback_.setEnabled(enabled); }

private:
    EStopImpl   &impl_;
    AbstractEStopCallback &callback_;
};

//////////////////////////////////////////////////////////////////////

EStopImpl::EStopImpl( AbstractEStopCallback        &callback,
                      const orca::EStopDescription &descr,
                      const std::string            &interfaceTag, 
                      const orcaice::Context       &context  ) :
    callback_(callback),
    description_(descr),
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

EStopImpl::EStopImpl( AbstractEStopCallback        &callback,
                      const orca::EStopDescription &descr,
                      const orcaice::Context       &context,
                      const std::string            &interfaceName ) :
    callback_(callback),
    description_(descr),
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
    topicHandler_.reset( new EStopTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );
    ptr_ = new EStopI( *this, callback_ );
}

void
EStopImpl::initInterface()
{
    topicHandler_->connectToTopic();
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
EStopImpl::initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName, int retryInterval )
{
    topicHandler_->connectToTopic( activity, subsysName, retryInterval );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
}

::orca::EStopData
EStopImpl::internalGetData()
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
        throw orca::SubscriptionFailedException("topicHandler_ does not exist.");
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

