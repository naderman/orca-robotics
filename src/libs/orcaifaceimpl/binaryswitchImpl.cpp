#include <iostream>
#include <orcaice/orcaice.h>
 
#include "binaryswitchImpl.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class BinarySwitchI : virtual public ::orca::BinarySwitch
{
public:

    BinarySwitchI( BinarySwitchImpl             &impl,
                   AbstractBinarySwitchCallback &callback )
        : impl_(impl),
          callback_(callback) {}
    virtual ::orca::BinarySwitchData getData(const Ice::Current&)
        { return impl_.internalGetData(); }
    virtual IceStorm::TopicPrx subscribe(const orca::BinarySwitchConsumerPrx& subscriber, const ::Ice::Current& = ::Ice::Current())
        { return impl_.internalSubscribe(subscriber); }

    virtual void setData( const orca::BinarySwitchData &data, const Ice::Current& )
        { callback_.setData(data); }

private:
    BinarySwitchImpl   &impl_;
    AbstractBinarySwitchCallback &callback_;
};

//////////////////////////////////////////////////////////////////////

BinarySwitchImpl::BinarySwitchImpl( AbstractBinarySwitchCallback &callback,
                                    const std::string            &interfaceTag, 
                                    const orcaice::Context       &context  ) :
    callback_(callback),
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

BinarySwitchImpl::BinarySwitchImpl( AbstractBinarySwitchCallback &callback,
                                    const orcaice::Context       &context,
                                    const std::string            &interfaceName ) :
    callback_(callback),
    interfaceName_(interfaceName),
    context_(context)
{
    init();
}

BinarySwitchImpl::~BinarySwitchImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
BinarySwitchImpl::init()
{
    topicHandler_.reset( new BinarySwitchTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );

    ptr_ = new BinarySwitchI( *this, callback_ );
}

void
BinarySwitchImpl::initInterface()
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    topicHandler_->connectToTopic();
}

void 
BinarySwitchImpl::initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName, int retryInterval )
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );

    topicHandler_->connectToTopic( activity, subsysName, retryInterval );
}

::orca::BinarySwitchData
BinarySwitchImpl::internalGetData() const
{
    context_.tracer().debug( "BinarySwitchImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::BinarySwitchData data;
    dataStore_.get( data );
    return data;
}


IceStorm::TopicPrx 
BinarySwitchImpl::internalSubscribe(const orca::BinarySwitchConsumerPrx& subscriber)
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
        orca::BinarySwitchData data;
        dataStore_.get( data );
    
        return topicHandler_->subscribe( subscriber, data );
    }
}

void
BinarySwitchImpl::localSetAndSend( const orca::BinarySwitchData& data )
{
    dataStore_.set( data );
    
    topicHandler_->publish( data );
}

}

