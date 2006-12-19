/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

StatusI::StatusI( const orcaice::Context & context )
          : context_(context),
            topic_(0),
            publisher_(0),
            file_(NULL),
            sysLogger_(NULL)
{
    prefix_ = orcaice::toString(context.name()) + ": ";
        
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
        std::string s = prefix_+": Failed to connect to an IceStorm status topic '"+
            orcaice::toString( fqTName )+"'\n" +
            "\tYou may allow to proceed by setting Orca.Tracer.RequireIceStorm=0.";
        initTracerError( s );
        // this should kill the app
        exit(1);
    }
    else 
    {
        statusPublisher = 0;
        std::string s = prefix_+": Failed to connect to an IceStorm status topic\n";
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
    initTracerPrint( prefix_+": Connecting to status topic "+orcaice::toString( fqTName ));

    try
    {
        topic_ = orcaice::connectToTopicWithString<orca::StatusConsumerPrx>(
            context_, publisher_, orcaice::toString( fqTName ) );
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( prefix_+": Caught exception while connecting to IceStorm: "+e.what() );
        icestormConnectFailed( fqTName, publisher_, isStatusTopicRequired );
    } // catch
    catch ( Ice::Exception &e )
    {
        std::stringstream s;
        s << prefix_ << ": Caught exception while connecting to IceStorm: " << e;
        initTracerError( s.str() );
        icestormConnectFailed( fqTName, publisher_, isStatusTopicRequired );
    }
    catch ( ... )
    {
        initTracerError( prefix_+": Caught unknown exception while connecting to IceStorm." );
        icestormConnectFailed( fqTName, publisher_, isStatusTopicRequired );
    }

    if ( publisher_ ) {
        initTracerPrint( prefix_+": Tracer connected to "+orcaice::toString(fqTName) );
    }

    return publisher_ != 0;
}

orca::StatusDataPtr
StatusI::getData(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;
    orca::StatusDataPtr currentStatus;
//     statusPipe_.get( currentStatus );
    // use the last status message, but update the time stamp.
    // does this make sense?
    orcaice::setToNow( currentStatus->timeStamp );
    
    return currentStatus;
}

void
StatusI::subscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !topic_ ) {
        throw orca::SubscriptionFailedException("Component does not have a topic to publish its status.");
    }
    
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;    
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

void
StatusI::status( const std::string &message, bool force )
{
    // get the current status and update it
    orca::StatusDataPtr currentStatus;
//     statusPipe_.get( currentStatus );

    if ( currentStatus->message != message )
    {
        currentStatus->message = message;
        orcaice::setToNow( currentStatus->timeStamp );
    }
    else
    {
        // the message has not changed, just update the time stamp
        orcaice::setToNow( currentStatus->timeStamp );
    }

    // stick it back into the data pipe
//     statusPipe_.set( currentStatus );
}

void 
StatusI::heartbeat( const std::string &message, int level )
{

}

std::string
StatusI::status() const
{
    orca::StatusDataPtr currentStatus;
//     statusPipe_.get( currentStatus );
    
    return currentStatus->message;
}
/*
void
StatusI::toTopic( const std::string& category, const std::string& message, int level )
{
    orca::StatusDataPtr statusData = new orca::StatusData;
    orcaice::setToNow( statusData->timeStamp );
    statusData->name = compName_;
    statusData->category = category;
    statusData->verbosity = level;
    statusData->message = message;

    IceUtil::Mutex::Lock lock(mutex_);
    
    // send data
    try
    {
        // This is very tricky, don't touch it if not sure
        // see Connections Issue 5, Avoiding Deadlocks, Part II, File Listing I.
        // If the lock is not released before the remote call, the program locks
        // up on the first remote call. 
        lock.release();
        publisher_->setData( statusData );
        lock.acquire();
        // end of tricky part.
    }
    catch ( const Ice::CommunicatorDestroyedException & ) // we are not using the exception
    {
        // it's ok, this is what happens on shutdown
        cout<<prefix_<<"tracer: communicator appears to be dead. We must be shutting down."<<endl;
        cout<<prefix_<<"tracer: unsent message: "<<orcaice::toString(statusData)<<endl;
    }
    catch ( const Ice::Exception &e )
    {
        cout<<prefix_<<"tracer: Caught exception while tracing to topic: "<<e<<endl;
        cout<<prefix_<<"tracer: unsent message: "<<orcaice::toString(statusData)<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
    catch ( ... )
    {
        cout<<prefix_<<"tracer: Caught unknown while tracing to topic."<<endl;
        cout<<prefix_<<"tracer: unsent message: "<<orcaice::toString(statusData)<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
}
*/
