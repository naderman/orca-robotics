/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "drivebicycleImpl.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

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

    virtual ::orca::DriveBicycleData getData(const ::Ice::Current& ) const
        { return impl_.internalGetData(); }

    virtual void subscribe(const ::orca::DriveBicycleConsumerPrx& subscriber,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( subscriber ); }

    virtual void unsubscribe(const ::orca::DriveBicycleConsumerPrx& subscriber,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( subscriber ); }

    virtual orca::VehicleDescription getDescription( const Ice::Current& c ) const
        { return impl_.internalGetDescription(); }

    virtual void setCommand(const ::orca::DriveBicycleCommand& command, 
                            const ::Ice::Current& current )
        {  impl_.internalSetCommand( command ); }

private:
    DriveBicycleImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

DriveBicycleImpl::DriveBicycleImpl( 
            const orca::VehicleDescription &descr,
            const std::string              &interfaceTag,
            const orcaice::Context         &context )
    : description_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

DriveBicycleImpl::DriveBicycleImpl( 
            const orca::VehicleDescription &descr,
            const orcaice::Context         &context,
            const std::string              &interfaceName )
    : description_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

DriveBicycleImpl::~DriveBicycleImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
DriveBicycleImpl::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithString<orca::DriveBicycleConsumerPrx>
        ( context_, consumerPrx_, topicName_ );

    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new DriveBicycleI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
DriveBicycleImpl::initInterface( hydroiceutil::Thread* thread, const std::string& subsysName, int retryInterval )
{
    topicPrx_ = orcaice::connectToTopicWithString<orca::DriveBicycleConsumerPrx>
        ( context_, consumerPrx_, topicName_, thread, subsysName, retryInterval );

    ptr_ = new DriveBicycleI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

::orca::DriveBicycleData 
DriveBicycleImpl::internalGetData() const
{
    context_.tracer().debug( "DriveBicycleImpl::internalGetData()", 5 );

    if ( dataPipe_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::DriveBicycleData data;
    dataPipe_.get( data );
    return data;
}

void
DriveBicycleImpl::internalSetCommand(const ::orca::DriveBicycleCommand& command )
{
    if ( this->hasNotifyHandler() ) {
        this->set( command );
    }
}

void 
DriveBicycleImpl::internalSubscribe(const ::orca::DriveBicycleConsumerPrx& subscriber)
{
    context_.tracer().debug( "DriveBicycleImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
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
        ss <<"DriveBicycleImpl::internalSubscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
DriveBicycleImpl::internalUnsubscribe(const ::orca::DriveBicycleConsumerPrx& subscriber)
{
    context_.tracer().debug( "DriveBicycleImpl::internalSubscribe(): unsubscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

void
DriveBicycleImpl::localSet( const orca::DriveBicycleData &data )
{
//     cout<<"TRACE(drivebicycleIface.cpp): localSet(): " << orcaice::toString(data) << endl;

    dataPipe_.set( data );
}

void
DriveBicycleImpl::localSetAndSend( const orca::DriveBicycleData &data )
{
//     cout<<"TRACE(drivebicycleiface.cpp): localSetAndSend(): " << orcaice::toString(data) << endl;

    dataPipe_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<orca::DriveBicycleConsumerPrx,orca::DriveBicycleData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          interfaceName_,
          topicName_ );
}

} // namespace
