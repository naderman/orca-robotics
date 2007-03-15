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

#include "tracerI.h"
#include "syslogger.h"

using namespace std;
using namespace orcaice::detail;

TracerI::TracerI( const orcaice::Context & context ) :
    LocalTracer(context),
    topic_(0),
    publisher_(0)
{
    // do we need IceStorm topic?
    if ( config_.verbosity[AnyTrace][ToNetwork] ) {
        connectToIceStorm();
    }
}

TracerI::~TracerI()
{
}

void
TracerI::icestormConnectFailed( const orca::FQTopicName &fqTName,
                                      orca::TracerConsumerPrx &publisher,
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
        publisher = 0;
        std::string s = prefix_+": Failed to connect to an IceStorm status topic\n";
        s += "\tAll trace messages will be local.\n";
        s += "\tYou may enforce connection by setting Orca.Tracer.RequireIceStorm=1.";
        initTracerWarning( s );

        // turn off all outputs toNetwork
        for ( int i=0; i<NumberOfTraceTypes; ++i ) {
            config_.verbosity[i][ToNetwork] = 0;
        }
        // move on
    }
}

bool
TracerI::connectToIceStorm()
{
    topic_     = 0;
    publisher_ = 0;

    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // are we required to connect to status topic? (there's always default value for this property)
    bool isStatusTopicRequired = props->getPropertyAsInt( "Orca.Tracer.RequireIceStorm" );

    // fqTName is something like "tracer/*@hostName/componentName"
    orca::FQTopicName fqTName = orcaice::toTracerTopic( context_.name() );
    initTracerPrint( prefix_+": Connecting to tracer topic "+orcaice::toString( fqTName ));

    try
    {
        topic_ = orcaice::connectToTopicWithString<orca::TracerConsumerPrx>(
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

::orca::TracerVerbosityConfig 
TracerI::getVerbosity( const ::Ice::Current& ) const
{
    ::orca::TracerVerbosityConfig config;

    config.info = config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork];
    config.warning = config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork];
    config.error = config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork];
    config.debug = config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork];

    return config;
}

void 
TracerI::setVerbosity( const ::orca::TracerVerbosityConfig& config,  const ::Ice::Current& )
{
    if ( config.info > -1 ) {
        config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork]     = config.info;
    }
    if ( config.warning > -1 ) {
        config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork]  = config.warning;
    }
    if ( config.error > -1 ) {
        config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork]    = config.error;
    }
    if ( config.debug > -1 ) {
        config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork]    = config.debug;
    }

    stringstream ss;
    ss << "New ToNetwork verbosity levels:"
        << " info="<<config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork]
        << " warn="<<config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork]
        << " error="<<config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork]
        << " debug="<<config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork];
    // do not call info because it may go back to the network and lock up
    // do it manually
    if ( config_.verbosity[InfoTrace][ToDisplay] ) {
        toDisplay( "info", ss.str(), 1 );
    }
    if ( config_.verbosity[InfoTrace][ToFile] ) {
        toFile( "info", ss.str(), 1 );
    }   
    if ( config_.verbosity[InfoTrace][ToLog] ) {
        sysLogger_->logInfo( ss.str() );
    }  

    // pre-calculate marginals: accross trace types
    recalcMarginals();
}

void
TracerI::subscribe(const ::orca::TracerConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !topic_ ) {
        if ( !connectToIceStorm() ) {
            throw orca::SubscriptionFailedException("Component does not have a topic to publish its traces.");
        }
    }
    
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;    
    qos["reliability"] = "twoway";
    topic_->subscribe( qos, subscriber );
}

void
TracerI::unsubscribe(const ::orca::TracerConsumerPrx& subscriber, const ::Ice::Current&)
{
    //cout<<"unsubscription request"<<endl;
    if ( topic_ ) {
        topic_->unsubscribe( subscriber );
    }
}

void
TracerI::info( const std::string &message, int level )
{
    LocalTracer::info( message, level );

    if ( config_.verbosity[InfoTrace][ToNetwork] >= level ) {
        toNetwork( "info", message, level );
    }
}

void
TracerI::warning( const std::string &message, int level )
{
    LocalTracer::warning( message, level );

    if ( config_.verbosity[WarningTrace][ToNetwork] >= level ) {
        toNetwork( "warn", message, level );
    }
}
    
void
TracerI::error( const std::string &message, int level )
{
    LocalTracer::error( message, level );

    if ( config_.verbosity[ErrorTrace][ToNetwork] >= level ) {
        toNetwork( "error", message, level );
    }
}

void
TracerI::debug( const std::string &message, int level )
{
    LocalTracer::debug( message, level );

    if ( config_.verbosity[DebugTrace][ToNetwork] >= level ) {
        toNetwork( "debug", message, level );
    }
}

void
TracerI::toNetwork( const std::string& category, const std::string& message, int level )
{
    IceUtil::Mutex::Lock lock(mutex_);

    orca::TracerData tracerData;
    orcaice::setToNow( tracerData.timeStamp );
    tracerData.name = context_.name();
    tracerData.category = category;
    tracerData.verbosity = level;
    tracerData.message = message;

    
    // send data
    try
    {
        // This is very tricky, don't touch it if not sure
        // see Connections Issue 5, Avoiding Deadlocks, Part II, File Listing I.
        // If the lock is not released before the remote call, the program locks
        // up on the first remote call. 
        lock.release();
        publisher_->setData( tracerData );
        lock.acquire();
        // end of tricky part.
    }
    catch ( const Ice::CommunicatorDestroyedException & ) // we are not using the exception
    {
        // it's ok, this is what happens on shutdown
        cout<<prefix_<<"tracer: communicator appears to be dead. We must be shutting down."<<endl;
        cout<<prefix_<<"tracer: unsent message: "<<orcaice::toString(tracerData)<<endl;
    }
    catch ( const Ice::Exception &e )
    {
        cout<<prefix_<<"tracer: Caught exception while tracing to topic: "<<e<<endl;
        cout<<prefix_<<"tracer: unsent message: "<<orcaice::toString(tracerData)<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
    catch ( ... )
    {
        cout<<prefix_<<"tracer: Caught unknown while tracing to topic."<<endl;
        cout<<prefix_<<"tracer: unsent message: "<<orcaice::toString(tracerData)<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
}
