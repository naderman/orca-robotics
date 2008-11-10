/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include "powerImpl.h"
 

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PowerI : public orca::Power
{
public:

    PowerI( PowerImpl &impl )
        : impl_(impl) {}
    virtual ~PowerI() {}

    // remote interface
    virtual ::orca::PowerData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }
    virtual void subscribe(const ::orca::PowerConsumerPrx& consumer, const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }
    virtual void unsubscribe(const ::orca::PowerConsumerPrx& consumer, const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }

private:
    PowerImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PowerImpl::PowerImpl( const std::string& interfaceTag, const orcaice::Context& context ) :
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

PowerImpl::PowerImpl( const orcaice::Context& context, const std::string& interfaceName ) : 
    interfaceName_(interfaceName),
    context_(context)
{
    init();
}

PowerImpl::~PowerImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
PowerImpl::init()
{
    topicHandler_.reset( new PowerTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );

    ptr_ = new PowerI( *this );
}

void
PowerImpl::initInterface()
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    topicHandler_->connectToTopic();
}

void 
PowerImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );

    topicHandler_->connectToTopic( thread, subsysName, retryInterval );
}

::orca::PowerData 
PowerImpl::internalGetData() const
{
    if ( dataStore_.isEmpty() ) {
        std::stringstream ss; ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PowerData data;
    dataStore_.get( data );
    return data;
}

void 
PowerImpl::internalSubscribe(const ::orca::PowerConsumerPrx& subscriber)
{
    topicHandler_->subscribe( subscriber );
}

void 
PowerImpl::internalUnsubscribe(const ::orca::PowerConsumerPrx& subscriber)
{
    topicHandler_->unsubscribe( subscriber );
}

void
PowerImpl::localSet( const orca::PowerData& data )
{
    dataStore_.set( data );
}

void
PowerImpl::localSetAndSend( const orca::PowerData& data )
{
    dataStore_.set( data );

    topicHandler_->publish( data );
}

} // namespace
