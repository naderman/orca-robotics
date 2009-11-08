/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/orcaice.h>
#include <orcaice/convertutils.h>
#include "propertiesImpl.h"
 

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class PropertiesI : public orca::Properties
{
public:
    PropertiesI( PropertiesImpl &impl )
        : impl_(impl) {}
    virtual ~PropertiesI() {}
    virtual ::orca::PropertiesData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }
    virtual void setData( const ::orca::PropertiesData &data, const ::Ice::Current& )
        { impl_.internalSetData( data ); }
    virtual IceStorm::TopicPrx subscribe(const orca::PropertiesConsumerPrx& subscriber, const ::Ice::Current& = ::Ice::Current())
        { return impl_.internalSubscribe(subscriber); }
private:
    PropertiesImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

PropertiesImpl::PropertiesImpl( const std::string& interfaceTag, const orcaice::Context& context ) :
    interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
    context_(context)
{
    init();
}

PropertiesImpl::PropertiesImpl( const orcaice::Context& context, const std::string& interfaceName ) : 
    interfaceName_(interfaceName),
    context_(context)
{
    init();
}


PropertiesImpl::~PropertiesImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
PropertiesImpl::init()
{
    topicHandler_.reset( new PropertiesTopicHandler( orcaice::toTopicAsString(context_.name(),interfaceName_), context_ ) );
    ptr_ = new PropertiesI( *this );
}

void
PropertiesImpl::initInterface()
{
    topicHandler_->connectToTopic();
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
PropertiesImpl::initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName, int retryInterval )
{
    topicHandler_->connectToTopic( activity, subsysName, retryInterval );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, activity, subsysName, retryInterval );
}

::orca::PropertiesData 
PropertiesImpl::internalGetData()
{
    context_.tracer().debug( "PropertiesImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::PropertiesData data;
    dataStore_.get( data );
    return data;
}

void
PropertiesImpl::internalSetData( const ::orca::PropertiesData &data )
{
    context_.tracer().debug( "PropertiesImpl::internalSetData()", 5 );

    remotelySetDataStore_.set( data );
}

IceStorm::TopicPrx 
PropertiesImpl::internalSubscribe(const orca::PropertiesConsumerPrx& subscriber)
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
        orca::PropertiesData data;
        dataStore_.get( data );
    
        return topicHandler_->subscribe( subscriber, data );
    }
}

void
PropertiesImpl::localSet( const orca::PropertiesData& data )
{
    dataStore_.set( data );
}

void
PropertiesImpl::localSetAndSend( const orca::PropertiesData& data )
{
    dataStore_.set( data );

    topicHandler_->publish( data );
}

} // namespace
