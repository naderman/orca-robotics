/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
// #include <orcaifacestring/status.h>
#include "statusImpl.h"
#include "componentstatusaggregator.h"

using namespace std;

namespace orcaice
{
namespace detail
{

class StatusI : public orca::Status
{
public:

    StatusI( StatusImpl &impl )
        : impl_(impl) {}
    virtual ~StatusI() {}

    // remote interface
    virtual ::orca::StatusData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }
    virtual void subscribe(const ::orca::StatusConsumerPrx& consumer,
                           const ::Ice::Current& = ::Ice::Current())
        { impl_.internalSubscribe( consumer ); }
    virtual void unsubscribe(const ::orca::StatusConsumerPrx& consumer,
                             const ::Ice::Current& = ::Ice::Current())
        { impl_.internalUnsubscribe( consumer ); }
    virtual IceStorm::TopicPrx topic(const ::Ice::Current& = ::Ice::Current())
        { return impl_.internalTopic(); }

private:
    StatusImpl &impl_;
};

//////////////////////////////////////////////////////////////

StatusImpl::StatusImpl( const orcaice::Context& context ) :
    hydroiceutil::LocalStatus( 
            context.tracer(),
            hydroutil::Properties( context.properties()->getPropertiesForPrefix("Orca.Status."),"Orca.Status.") ),
    topicHandler_(0),
    interfaceName_("status"),
    context_(context)
{
    aggregator_.reset( new ComponentStatusAggregator );

    // fill the store
    orca::StatusData data;
    orcaice::setToNow( data.timeStamp );
    data.compStatus.name = context_.name();
    data.compStatus.timeUp = 0;
    data.compStatus.state = orca::CompInitialising;
    data.compStatus.health = orca::CompOk;
    dataStore_.set( data );

    initTracerInfo( "Status created" );
}

StatusImpl::~StatusImpl()
{
    if ( topicHandler_ )  delete topicHandler_;

//     orcaice::tryRemoveAdminInterface( context_, "Status" );
}

void
StatusImpl::initInterface()
{
// cout<<"DEBUG: "<<__func__<<"()"<<endl;
    initTopicHandler();

    ptr_ = new StatusI( *this );

    // previous method: adding Home to the component adapter
//     orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    // EXPERIMENTAL! adding as a facet to the Admin interface.
    orcaice::createAdminInterface( context_, ptr_, context_.name() );
}

void
StatusImpl::initTopicHandler()
{
// cout<<"DEBUG: "<<__func__<<"()"<<endl;
    if ( topicHandler_ ) {
        delete topicHandler_; 
        topicHandler_ = 0;
    }

    // are we required to connect to status topic? (there's always default value for this property)
    bool isTopicRequired = context_.properties()->getPropertyAsInt( "Orca.Status.RequireIceStorm" );

    // fqTName is something like "tracer/*@platformName/componentName"
    orca::FQTopicName fqTName = orcaice::toStatusTopic( context_.name() );

    topicHandler_ = new StatusTopicHandler( orcaice::toString(fqTName), context_ );
    if ( !topicHandler_->connectToTopic() )
    {
        if ( isTopicRequired ) 
        {
            std::string s = " Failed to connect to an IceStorm status topic '"+
                orcaice::toString(fqTName)+"'\n" +
                "\tYou may allow to proceed by setting Orca.Status.RequireIceStorm=0.";
            initTracerError( s );
            // this should kill the app
            context_.shutdown();
        }
        else 
        {
            std::string s = " Failed to connect to an IceStorm status topic\n";
            s += "\tAll trace messages will be local.\n";
            s += "\tYou may enforce connection by setting Orca.Status.RequireIceStorm=1.";
            initTracerWarning( s );
            // move on
        }

        delete topicHandler_; 
        topicHandler_ = 0;
    }
}

void 
StatusImpl::publishEvent( const hydroiceutil::LocalComponentStatus& componentStatus )
{
    orca::StatusData data;
    orcaice::setToNow( data.timeStamp );
    aggregator_->convert( componentStatus, context_.name(), data.compStatus );
    data.compStatus.timeUp = (Ice::Int)upTimer_.elapsedSec();

    dataStore_.set( data );

    if( topicHandler_ != 0 )
    {
// cout<<"DEBUG: topicHandler_="<<(int)topicHandler_<<endl;
        topicHandler_->publish( data );
    }
}

::orca::StatusData 
StatusImpl::internalGetData() const
{
//     context_.tracer().debug( "StatusImpl::internalGetData()", 5 );

    if ( dataStore_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (interface="<<interfaceName_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    orca::StatusData data;
    dataStore_.get( data );

    // Just update the timeUp
    data.compStatus.timeUp = (Ice::Int)upTimer_.elapsedSec();

    return data;
}

void 
StatusImpl::internalSubscribe(const ::orca::StatusConsumerPrx& subscriber)
{
// cout<<"DEBUG: "<<__func__<<"()"<<endl;
    if ( !topicHandler_ ) 
    {
        initTopicHandler();
        if ( !topicHandler_ )
        {
            throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
        }
    }
    
    // send all the information we have to the new subscriber (and to no one else)
    orca::StatusData data;
    dataStore_.get( data );

    topicHandler_->subscribe( subscriber, data );
}

void 
StatusImpl::internalUnsubscribe(const ::orca::StatusConsumerPrx& subscriber)
{
    if ( topicHandler_ )
        topicHandler_->unsubscribe( subscriber );
}

IceStorm::TopicPrx 
StatusImpl::internalTopic()
{
    return topicHandler_->topic();
}

}
}
