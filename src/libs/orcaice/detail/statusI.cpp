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

using namespace std;

using namespace orcaice::detail;


void 
StatusI::convert( const LocalStatus::SubsystemsStatus& internal, orca::SubsystemsStatus& network )
{
    IceUtil::Time t = IceUtil::Time::now();
    IceUtil::Time dt;
    
    for ( LocalStatus::SubsystemsStatus::const_iterator it=internal.begin(); it!=internal.end(); ++it ) 
    {
        switch ( it->second.type ) {
        case Status::Ok :
            network[it->first].type = orca::SubsystemStatusOk;
            break;
        case Status::Warning :
            network[it->first].type = orca::SubsystemStatusWarning;
            break;
        case Status::Fault :
            network[it->first].type = orca::SubsystemStatusFault;
            break;
        }
        network[it->first].message = it->second.message;
        dt = t - it->second.lastHeartbeat;
        network[it->first].sinceHeartbeat = dt.toSecondsDouble() / it->second.maxHeartbeatInterval;
    }
}

StatusI::StatusI( const orcaice::Context & context ) : 
    LocalStatus(context),
    topic_(0),
    publisher_(0)
{        
    // do we need IceStorm topic?
    bool needIceStorm = false;
    if ( needIceStorm ) {
        connectToIceStorm();
    }
}

StatusI::~StatusI()
{
}

void
StatusI::icestormConnectFailed( const orca::FQTopicName &fqTName,
                                      orca::StatusConsumerPrx &statusPublisher,
                                      bool isStatusTopicRequired )
{
    if ( isStatusTopicRequired ) 
    {
        std::string s = "Failed to connect to an IceStorm status topic '"+
            orcaice::toString( fqTName )+"'\n" +
            "\tYou may allow to proceed by setting Orca.Tracer.RequireIceStorm=0.";
        initTracerError( s );
        // this should kill the app
        exit(1);
    }
    else 
    {
        statusPublisher = 0;
        std::string s = "Failed to connect to an IceStorm status topic\n";
        s += "\tAll trace messages will be local.\n";
        s += "\tYou may enforce connection by setting Orca.Tracer.RequireIceStorm=1.";
        initTracerWarning( s );
    }
}

bool
StatusI::connectToIceStorm()
{
    topic_     = 0;
    publisher_ = 0;

    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // are we required to connect to status topic? (there's always default value for this property)
    bool isStatusTopicRequired = props->getPropertyAsInt( "Orca.Tracer.RequireIceStorm" );

    orca::FQTopicName fqTName = orcaice::toStatusTopic( context_.name() );
    initTracerPrint( "Connecting to status topic "+orcaice::toString( fqTName ));

    try
    {
        topic_ = orcaice::connectToTopicWithString<orca::StatusConsumerPrx>(
            context_, publisher_, orcaice::toString( fqTName ) );
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( std::string("Caught exception while connecting to IceStorm: ")+e.what() );
        icestormConnectFailed( fqTName, publisher_, isStatusTopicRequired );
    } // catch
    catch ( Ice::Exception &e )
    {
        std::stringstream s;
        s << "Caught exception while connecting to IceStorm: " << e;
        initTracerError( s.str() );
        icestormConnectFailed( fqTName, publisher_, isStatusTopicRequired );
    }
    catch ( ... )
    {
        initTracerError( "Caught unknown exception while connecting to IceStorm." );
        icestormConnectFailed( fqTName, publisher_, isStatusTopicRequired );
    }

    if ( publisher_ ) {
        initTracerPrint( "Tracer connected to "+orcaice::toString(fqTName) );
    }

    return publisher_ != 0;
}

orca::StatusData
StatusI::getData(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;

    IceUtil::Mutex::Lock lock(mutex_);

    orca::StatusData status;
    orcaice::setToNow( status.timeStamp );

    status.name = context_.name();

    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    status.timeUp = timeUp.toSeconds();

    convert( subsystems_, status.subsystems );
    
    return status;
}

void
StatusI::subscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !topic_ ) {
        throw orca::SubscriptionFailedException("Component does not have a topic to publish its status.");
    }
    
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;    
    qos["reliability"] = "twoway";
    topic_->subscribe( qos, subscriber );
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

// void 
// StatusI::heartbeat( const std::string& subsystem )
// {
// }
//     
// void 
// StatusI::status( const std::string& subsystem, SubsystemStatusType type, const std::string& message )
// {
// }
// 
// IceUtil::Time 
// StatusI::startTime() const
// {
// }
