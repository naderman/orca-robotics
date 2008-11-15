/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "drivebicycleImpl.h"
 
using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class DriveBicycleI : public orca::DriveBicycle
{
public:
    DriveBicycleI( DriveBicycleImpl &impl )
        : impl_(impl) {}
    virtual orca::VehicleDescription getDescription( const Ice::Current& )
        { return impl_.internalGetDescription(); }
    virtual ::orca::DriveBicycleData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }
        virtual IceStorm::TopicPrx subscribe(const orca::DriveBicycleConsumerPrx& subscriber, const ::Ice::Current& = ::Ice::Current())
            { return impl_.internalSubscribe(subscriber); }
    virtual void setCommand(const ::orca::DriveBicycleCommand& command, const ::Ice::Current& current )
        {  impl_.internalSetCommand( command ); }
private:
    DriveBicycleImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

DriveBicycleImpl::DriveBicycleImpl( 
            const orca::VehicleDescription &descr,
            const std::string              &interfaceTag,
            const orcaice::Context         &context ) : 
    description_(descr),
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

DriveBicycleImpl::DriveBicycleImpl( 
            const orca::VehicleDescription &descr,
            const orcaice::Context         &context,
            const std::string              &interfaceName ) : 
    description_(descr),
    interfaceName_(interfaceName),
    context_(context)
{
    init();
}

DriveBicycleImpl::~DriveBicycleImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
DriveBicycleImpl::init()
{
    ptr_ = new DriveBicycleI( *this );

    topicHandler_.reset( new DriveBicycleTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );
}

void
DriveBicycleImpl::initInterface()
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    topicHandler_->connectToTopic();
}

void 
DriveBicycleImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );

    topicHandler_->connectToTopic( thread, subsysName, retryInterval );
}

::orca::DriveBicycleData 
DriveBicycleImpl::internalGetData() const
{
    context_.tracer().debug( "DriveBicycleImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::DriveBicycleData data;
    dataStore_.get( data );
    return data;
}

void
DriveBicycleImpl::internalSetCommand(const ::orca::DriveBicycleCommand& command )
{
    if ( this->hasNotifyHandler() ) {
        this->set( command );
    }
}

IceStorm::TopicPrx 
DriveBicycleImpl::internalSubscribe(const orca::DriveBicycleConsumerPrx& subscriber)
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
        orca::DriveBicycleData data;
        dataStore_.get( data );
    
        return topicHandler_->subscribe( subscriber, data );
    }
}

void
DriveBicycleImpl::localSet( const orca::DriveBicycleData &data )
{
    dataStore_.set( data );
}

void
DriveBicycleImpl::localSetAndSend( const orca::DriveBicycleData &data )
{
    dataStore_.set( data );

    topicHandler_->publish( data );
}

} // namespace
