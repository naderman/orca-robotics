/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

#include "statusI.h"
#include "syslogger.h"
#include "../status.h"

using namespace std;

using namespace orcaice::detail;

StatusI::StatusI( const orcaice::Context & context )
    : topic_(0),
      publisher_(0),
      startTime_(IceUtil::Time::now()),
      context_(context)
{
    IceUtil::Mutex::Lock lock(mutex_);

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

StatusI::~StatusI()
{
}

void
StatusI::setStatusData( const std::map<std::string,LocalStatus::SubsystemStatus> &subsystemStatus )
{
    orcaice::setToNow( statusData_.timeStamp );

    statusData_.name = context_.name();

    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    statusData_.timeUp = timeUp.toSeconds();

    convert( subsystemStatus, statusData_.subsystems );
}

void 
StatusI::localSetData( const std::map<std::string,LocalStatus::SubsystemStatus> &subsystemStatus )
{
    IceUtil::Mutex::Lock lock(mutex_);

    setStatusData( subsystemStatus );
    sendToIceStorm( statusData_ );
}

void
StatusI::sendToIceStorm( const orca::StatusData &statusData )
{
    // send data
    try
    {
        publisher_->setData( statusData );
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
    topic_     = 0;
    publisher_ = 0;

    try
    {
        topic_ = orcaice::connectToTopicWithString<orca::StatusConsumerPrx>(
            context_, publisher_, topicName_ );
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( std::string("Caught exception while connecting to IceStorm: ")+e.what() );
        icestormConnectFailed( topicName_, publisher_, isStatusTopicRequired_ );
    } // catch
    catch ( Ice::Exception &e )
    {
        std::stringstream s;
        s << "Caught exception while connecting to IceStorm: " << e;
        initTracerError( s.str() );
        icestormConnectFailed( topicName_, publisher_, isStatusTopicRequired_ );
    }
    catch ( ... )
    {
        initTracerError( "Caught unknown exception while connecting to IceStorm." );
        icestormConnectFailed( topicName_, publisher_, isStatusTopicRequired_ );
    }

    if ( publisher_ ) {
        initTracerInfo( context_.tag()+": Status connected to "+topicName_ );
    }

    return publisher_ != 0;
}

orca::StatusData
StatusI::getData(const ::Ice::Current& ) const
{
    IceUtil::Mutex::Lock lock(mutex_);

    // Just update the timeUp
    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    statusData_.timeUp = timeUp.toSeconds();
    
    return statusData_;
}

void
StatusI::subscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !topic_ ) {
        throw orca::SubscriptionFailedException("Component does not have a topic to publish its status.");
    }
    
    try {
        //cout<<"subscription request"<<endl;
        topic_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Failed to subscribe: "<< e;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
StatusI::unsubscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !topic_ ) {
        return;
    }
    
    //cout<<"unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}



void StatusI::convert( const std::map<std::string,LocalStatus::SubsystemStatus> &internal,
                       orca::SubsystemsStatus &network ) const
{
    IceUtil::Time t = IceUtil::Time::now();
    IceUtil::Time dt;

    std::map<std::string,LocalStatus::SubsystemStatus>::const_iterator it;
    
    for ( it=internal.begin(); it!=internal.end(); ++it ) 
    {
        switch ( it->second.type ) {
        case orcaice::Status::Initialising :
            network[it->first].type = orca::SubsystemStatusInitialising;
            break;
        case orcaice::Status::Ok :
            network[it->first].type = orca::SubsystemStatusOk;
            break;
        case orcaice::Status::Warning :
            network[it->first].type = orca::SubsystemStatusWarning;
            break;
        case orcaice::Status::Fault :
            network[it->first].type = orca::SubsystemStatusFault;
            break;
        case orcaice::Status::Stalled :
            network[it->first].type = orca::SubsystemStatusStalled;
            break;
        }
        network[it->first].message = it->second.message;
        dt = t - it->second.lastHeartbeatTime;
        network[it->first].sinceHeartbeat = dt.toSecondsDouble() / it->second.maxHeartbeatInterval;
    }
}

