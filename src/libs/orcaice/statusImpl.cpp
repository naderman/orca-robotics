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
// #include <orcaifacestring/status.h>
#include "statusImpl.h"

using namespace std;

namespace orcaice
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
    interfaceName_("status"),
    topicName_( orcaice::toString( orcaice::toStatusTopic( context.name() ) ) ),
    context_(context)
{
    // fill the store
    orca::StatusData data;
    orcaice::setToNow( data.timeStamp );
    dataStore_.set( data );

    initTracerInfo( "Status created" );
}

StatusImpl::~StatusImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
StatusImpl::initInterface()
{
    Ice::PropertiesPtr props = context_.properties();
    // are we required to connect to status topic? (there's always default value for this property)
    bool isStatusTopicRequired = props->getPropertyAsInt( "Orca.Status.RequireIceStorm" );

    // Find IceStorm Topic to which we'll publish
    try
    {
        topicPrx_ = orcaice::connectToTopicWithString<orca::StatusConsumerPrx>
            ( context_, publisherPrx_, topicName_ );
    }
    // we only catch the exception which would be thrown if IceStorm is not there.
    catch ( const orcaice::NetworkException& e )
    {
        if ( isStatusTopicRequired )
        {
            std::string s = "Failed to connect to an IceStorm status topic '"+
                topicName_+"'\n" +
                "\tYou may allow to proceed by setting Orca.Status.RequireIceStorm=0.";
            initTracerError( s );
            context_.shutdown();
        }
        else
        {
            std::string s = "Failed to connect to an IceStorm status topic\n";
            s += "\tAll status messages will be local.\n";
            s += "\tYou may enforce connection by setting Orca.Status.RequireIceStorm=1.";
            initTracerWarning( s );
        }
    }

    ptr_ = new StatusI( *this );

    // previous method: adding Home to the component adapter
//     orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    // EXPERIMENTAL! adding as a facet to the Admin interface.
    try
    {
        context_.communicator()->addAdminFacet( ptr_, "Status" );
    }
    catch ( const std::exception& e )
    {
        stringstream ss;
        ss << "(while installng Status object) : "<<e.what();
        context_.tracer().error( ss.str() );
        context_.shutdown();
    }

    // manually to home registry
    orca::ProvidedInterface iface;
    iface.name = interfaceName_;
    iface.id   = "::orca::Status";
    context_.home().addProvidedInterface( iface );
}

void 
StatusImpl::publishEvent( const hydroiceutil::LocalComponentStatus& componentStatus )
{
    if ( !publisherPrx_ )
        return;

    orca::StatusData data;
    orcaice::setToNow( data.timeStamp );
    aggregator_.convert( componentStatus, context_.name(), data.compStatus );
    data.compStatus.timeUp = (Ice::Int)upTimer_.elapsedSec();

    dataStore_.set( data );

    // Try to push to IceStorm.
    orcaice::tryPushToIceStormWithReconnect<orca::StatusConsumerPrx,orca::StatusData> ( 
        context_,
        publisherPrx_,
        data,
        topicPrx_,
        topicName_ );
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
    if ( !topicPrx_ )
        throw orca::SubscriptionFailedException( "Not connected to topic yet" );

    context_.tracer().debug( "StatusImpl::internalSubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    // see Ice Manual sec.45.6 "Publishing to a specific subscriber"
    orca::StatusConsumerPrx individualPublisher;
    try {
        // this talks to IceStorm
        Ice::ObjectPrx pub = topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
        individualPublisher = orca::StatusConsumerPrx::uncheckedCast(pub);
    }
    catch ( const IceStorm::AlreadySubscribed& e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e.what();
        context_.tracer().debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception& e ) {
        std::stringstream ss;
        ss <<"StatusImpl::internalSubscribe: failed to subscribe: "<< e.what() << endl;
        context_.tracer().warning( ss.str() );
        // throws exception back to the subscriber
        throw orca::SubscriptionFailedException( ss.str() );
    }

    // send all the information we have to the new subscriber (and to no one else)
    orca::StatusData data;
    dataStore_.get( data );

    // Just update the timeUp
    data.compStatus.timeUp = (Ice::Int)upTimer_.elapsedSec();

    try
    {
        // this talks to IceStorm
        individualPublisher->setData( data );   
    }
    catch ( const Ice::Exception&  e ) {
        std::stringstream ss;
        ss <<"StatusImpl::subscribe: failed to send information to the new subscriber: "<< e.what() << endl;
        context_.tracer().warning( ss.str() );
        // throws exception back to the subscriber
        throw orca::OrcaException( ss.str() );
    }
    cout<<"StatusImpl::subscribe(): sent status info to new subscriber: "<<individualPublisher->ice_toString()<<endl;
//     cout<<ifacestring::toString( statusData_ )<<endl;
}

void 
StatusImpl::internalUnsubscribe(const ::orca::StatusConsumerPrx& subscriber)
{
    if ( !topicPrx_ )
        return;

    context_.tracer().debug( "StatusImpl::internalUnsubscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    topicPrx_->unsubscribe( subscriber );
}

IceStorm::TopicPrx 
StatusImpl::internalTopic()
{
    return topicPrx_;
}

}
