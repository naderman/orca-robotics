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
#include <orcaifacestring/status.h>
#include "statusI.h"

using namespace std;
using namespace orcaice::detail;

namespace
{

void 
convert( const hydroiceutil::NameStatusMap &internal, orca::SubsystemsStatus &network )
{
    hydroiceutil::NameStatusMap::const_iterator it;
    
    for ( it=internal.begin(); it!=internal.end(); ++it ) 
    {
        switch ( it->second.type ) 
        {
        case gbxutilacfr::SubsystemStatusInitialising :
            network[it->first].type = orca::SubsystemStatusInitialising;
            break;
        case gbxutilacfr::SubsystemStatusOk :
            network[it->first].type = orca::SubsystemStatusOk;
            break;
        case gbxutilacfr::SubsystemStatusWarning :
            network[it->first].type = orca::SubsystemStatusWarning;
            break;
        case gbxutilacfr::SubsystemStatusFault :
            network[it->first].type = orca::SubsystemStatusFault;
            break;
        case gbxutilacfr::SubsystemStatusStalled :
            network[it->first].type = orca::SubsystemStatusStalled;
            break;
        }
        network[it->first].message = it->second.message;
        network[it->first].sinceHeartbeat = 
            (Ice::Float)(it->second.heartbeatTimer.elapsedSec() / it->second.maxHeartbeatInterval);
    }
}

} // namespace

StatusI::StatusI( const orcaice::Context& context ) :
    hydroiceutil::LocalStatus( 
            context.tracer(),
            hydroutil::Properties( context.properties()->getPropertiesForPrefix("Orca.Status."),"Orca.Status.") ), 
    topicPrx_(0),
    publisherPrx_(0),
    context_(context)
{
//     IceUtil::Mutex::Lock lock(mutex_);

    orca::FQTopicName fqTName = orcaice::toStatusTopic( context_.name() );
    topicName_ = orcaice::toString( fqTName );

    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // are we required to connect to status topic? (there's always default value for this property)
    isStatusTopicRequired_ = props->getPropertyAsInt( "Orca.Tracer.RequireIceStorm" );

    connectToIceStorm();

    // init statusData
    statusData_.timeStamp = orcaice::getNow();
    statusData_.name = context_.name();
    statusData_.timeUp  = 0;
}

void 
StatusI::publishEvent( const hydroiceutil::NameStatusMap& subsystems )
{
    // protect status data structure
    IceUtil::Mutex::Lock lock(mutex_);

    orcaice::setToNow( statusData_.timeStamp );

    statusData_.name = context_.name();
    statusData_.timeUp = (Ice::Int)upTimer_.elapsedSec();

    convert( subsystems, statusData_.subsystems );

    sendToIceStorm( statusData_ );
}

void
StatusI::sendToIceStorm( const orca::StatusData &statusData )
{
    // send data
    try
    {
        publisherPrx_->setData( statusData );
    }
    catch ( const Ice::CommunicatorDestroyedException & ) // we are not using the exception
    {
        // it's ok, this is what happens on shutdown
    }
    catch ( const Ice::Exception &e )
    {
        cout<<orcaice::toString(context_.name())<<": status: Caught exception while sending status to topic: "<<e<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
    catch ( ... )
    {
        cout<<orcaice::toString(context_.name())<<": status: Caught unknown while sending status to topic."<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
}

void
StatusI::icestormConnectFailed( const std::string &topicName,
                                orca::StatusConsumerPrx &statusPublisher,
                                bool isStatusTopicRequired )
{
    if ( isStatusTopicRequired ) 
    {
        std::string s = "Failed to connect to an IceStorm status topic '"+
            topicName+"'\n" +
            "\tYou may allow to proceed by setting Orca.Status.RequireIceStorm=0.";
        initTracerError( s );
        // this should kill the app
        exit(1);
    }
    else 
    {
        statusPublisher = 0;
        std::string s = "Failed to connect to an IceStorm status topic\n";
        s += "\tAll trace messages will be local.\n";
        s += "\tYou may enforce connection by setting Orca.Status.RequireIceStorm=1.";
        initTracerWarning( s );
    }
}

bool
StatusI::connectToIceStorm()
{
    topicPrx_     = 0;
    publisherPrx_ = 0;

    try
    {
        topicPrx_ = orcaice::connectToTopicWithString<orca::StatusConsumerPrx>(
            context_, publisherPrx_, topicName_ );
    }
    catch ( const gbxutilacfr::Exception & e )
    {
        initTracerError( std::string("Caught exception while connecting to IceStorm: ")+e.what() );
        icestormConnectFailed( topicName_, publisherPrx_, isStatusTopicRequired_ );
    } // catch
    catch ( Ice::Exception &e )
    {
        std::stringstream s;
        s << "Caught exception while connecting to IceStorm: " << e;
        initTracerError( s.str() );
        icestormConnectFailed( topicName_, publisherPrx_, isStatusTopicRequired_ );
    }
    catch ( ... )
    {
        initTracerError( "Caught unknown exception while connecting to IceStorm." );
        icestormConnectFailed( topicName_, publisherPrx_, isStatusTopicRequired_ );
    }

    if ( publisherPrx_ ) {
        initTracerInfo( context_.tag()+": Status connected to "+topicName_ );
    }

    return publisherPrx_ != 0;
}

orca::StatusData
StatusI::getData(const ::Ice::Current& ) const
{
    IceUtil::Mutex::Lock lock(mutex_);

    // Just update the timeUp
    statusData_.timeUp = (Ice::Int)upTimer_.elapsedSec();
    
    return statusData_;
}

void
StatusI::subscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer().debug( "StatusI::subscribe(): subscriber='"+subscriber->ice_toString()+"'", 4 );
    // see Ice Manual sec.45.6 "Publishing to a specific subscriber"
    orca::StatusConsumerPrx individualPublisher;
    try {
        Ice::ObjectPrx pub = topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
        individualPublisher = orca::StatusConsumerPrx::uncheckedCast(pub);
    }
    catch ( const IceStorm::AlreadySubscribed&  e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().info( ss.str() );    
        return;
    }
    catch ( const Ice::Exception&  e ) {
        std::stringstream ss;
        ss <<"StatusI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }

    // send all the information we have to the new subscriber (and to no one else)
    IceUtil::Mutex::Lock lock(mutex_);
    // Just update the timeUp
    statusData_.timeUp = (Ice::Int)upTimer_.elapsedSec();

    try
    {
        individualPublisher->setData( statusData_ );   
    }
    catch ( const Ice::Exception&  e ) {
        std::stringstream ss;
        ss <<"StatusI::subscribe: failed to send information to the new subscriber: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::OrcaException( ss.str() );
    }
    cout<<"StatusI::subscribe(): sent status info to new subscriber: "<<individualPublisher->ice_toString()<<endl;
    cout<<ifacestring::toString( statusData_ )<<endl;
}

void
StatusI::unsubscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !topicPrx_ ) {
        return;
    }
    
    //cout<<"unsubscription request"<<endl;
    topicPrx_->unsubscribe( subscriber );
}

