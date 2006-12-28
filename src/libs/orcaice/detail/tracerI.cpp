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

#include "tracerI.h"
#include "syslogger.h"

using namespace std;
using namespace orcaice::detail;

TracerI::TracerI( const orcaice::Context & context )
          : context_(context),
            topic_(0),
            publisher_(0),
            file_(NULL),
            sysLogger_(NULL)
{
    prefix_ = orcaice::toString(context.name()) + ": ";
    parseConfigFile();
        
    // do we need IceStorm topic?
    if ( config_.verbosity[AnyTrace][ToNetwork] ) {
        connectToIceStorm();
    }

    // do we need output file?
    if ( config_.verbosity[AnyTrace][ToFile] ) {
        string filename = context_.properties()->getProperty("Orca.Tracer.Filename");
        file_ = new ofstream( filename.c_str() );
        if ( !file_->is_open() ) {
            initTracerError( "Could not create file " + filename );
        }
        else {
            initTracerPrint("Created output file.");
        }
    }
    
    // do we need a syslogger?
    if ( config_.verbosity[AnyTrace][ToLog] ) {
        sysLogger_ = new orcaice::detail::SysLogger(context);
        initTracerPrint("Created syslogger.");
    }
}

TracerI::~TracerI()
{
    if ( sysLogger_ ) delete sysLogger_;
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

void
TracerI::parseConfigFile()
{
    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // set tracer configuration parameters
    // defaults were already set on all tracer properties
    
    // destination (defaults are zeros)
    config_.verbosity[TracerI::InfoTrace][TracerI::ToDisplay]     = props->getPropertyAsInt("Orca.Tracer.InfoToDisplay");
    config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork]     = props->getPropertyAsInt("Orca.Tracer.InfoToNetwork");
    config_.verbosity[TracerI::InfoTrace][TracerI::ToLog]         = props->getPropertyAsInt("Orca.Tracer.InfoToLog");
    config_.verbosity[TracerI::InfoTrace][TracerI::ToFile]        = props->getPropertyAsInt("Orca.Tracer.InfoToFile");

    config_.verbosity[TracerI::WarningTrace][TracerI::ToDisplay]  = props->getPropertyAsInt("Orca.Tracer.WarningToDisplay");
    config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork]  = props->getPropertyAsInt("Orca.Tracer.WarningToNetwork");
    config_.verbosity[TracerI::WarningTrace][TracerI::ToLog]      = props->getPropertyAsInt("Orca.Tracer.WarningToLog");
    config_.verbosity[TracerI::WarningTrace][TracerI::ToFile]     = props->getPropertyAsInt("Orca.Tracer.WarningToFile");

    config_.verbosity[TracerI::ErrorTrace][TracerI::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.ErrorToDisplay");
    config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork]    = props->getPropertyAsInt("Orca.Tracer.ErrorToNetwork");
    config_.verbosity[TracerI::ErrorTrace][TracerI::ToLog]        = props->getPropertyAsInt("Orca.Tracer.ErrorToLog");
    config_.verbosity[TracerI::ErrorTrace][TracerI::ToFile]       = props->getPropertyAsInt("Orca.Tracer.ErrorToFile");

    config_.verbosity[TracerI::DebugTrace][TracerI::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.DebugToDisplay");
    config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork]    = props->getPropertyAsInt("Orca.Tracer.DebugToNetwork");
    config_.verbosity[TracerI::DebugTrace][TracerI::ToLog]        = props->getPropertyAsInt("Orca.Tracer.DebugToLog");
    config_.verbosity[TracerI::DebugTrace][TracerI::ToFile]       = props->getPropertyAsInt("Orca.Tracer.DebugToFile");

    // pre-calculate marginals: accross trace types
    recalcMarginals();

    // filtering
    config_.ignoreRepeatedWarnings = props->getPropertyAsInt( "Orca.Tracer.IgnoreRepeatedWarnings" );
    config_.ignoreRepeatedErrors   = props->getPropertyAsInt( "Orca.Tracer.IgnoreRepeatedErrors" );
    // format
    config_.addTimestamp           = props->getPropertyAsInt( "Orca.Tracer.Timestamp" );
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

void 
TracerI::setVerbosity( ::Ice::Int error, ::Ice::Int warn, ::Ice::Int info, ::Ice::Int debug,  const ::Ice::Current& )
{
    if ( info > -1 ) {
        config_.verbosity[TracerI::InfoTrace][TracerI::ToNetwork]     = info;
    }
    if ( warn > -1 ) {
        config_.verbosity[TracerI::WarningTrace][TracerI::ToNetwork]  = warn;
    }
    if ( error > -1 ) {
        config_.verbosity[TracerI::ErrorTrace][TracerI::ToNetwork]    = error;
    }
    if ( debug > -1 ) {
        config_.verbosity[TracerI::DebugTrace][TracerI::ToNetwork]    = debug;
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
TracerI::print( const std::string &message )
{
    IceUtil::Mutex::Lock lock(mutex_);
    cout<<message<<endl;
}

void
TracerI::info( const std::string &message, int level )
{
    if ( config_.verbosity[InfoTrace][ToDisplay] >= level ) {
        toDisplay( "info", message, level );
    }
    if ( config_.verbosity[InfoTrace][ToNetwork] >= level ) {
        toNetwork( "info", message, level );
    }
    if ( config_.verbosity[InfoTrace][ToFile] >= level ) {
        toFile( "info", message, level );
    }   
    if ( config_.verbosity[InfoTrace][ToLog] >= level ) {
        sysLogger_->logInfo( message );
    }   
}

void
TracerI::warning( const std::string &message, int level )
{
    if ( config_.verbosity[WarningTrace][ToDisplay] >= level ) {
        toDisplay( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToNetwork] >= level ) {
        toNetwork( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToFile] >= level ) {
        toFile( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToLog] >= level ) {
        sysLogger_->logWarning( message );
    }
}
    
void
TracerI::error( const std::string &message, int level )
{
    if ( config_.verbosity[ErrorTrace][ToDisplay] >= level ) {
        toDisplay( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToNetwork] >= level ) {
        toNetwork( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToFile] >= level ) {
        toFile( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToLog] >= level ) {
        sysLogger_->logError( message );
    }
}

void
TracerI::debug( const std::string &message, int level )
{
    if ( config_.verbosity[DebugTrace][ToDisplay] >= level ) {
        toDisplay( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToNetwork] >= level ) {
        toNetwork( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToFile] >= level ) {
        toFile( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToLog] >= level ) {
        sysLogger_->logDebug( message );
    }
}

int 
TracerI::verbosity( TraceType traceType, DestinationType destType ) const
{
    return 1;
}

void
TracerI::toDisplay( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    cout << output << endl;
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

void
TracerI::toFile( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    (*file_) << output << endl;
}

void
TracerI::assembleMessage( const std::string& category, const std::string& message, int level, std::string& s )
{
    s = "[ ";
    if(config_.addTimestamp)
    {
        s += IceUtil::Time::now().toString() + " ";
    }
    s += prefix_;
    // todo: would be nice to show level here, e.g. debug3:
    // but seems like an overkill to deal with osstreams
    s += " " + category + ": ";
    s += message + " ]";

    // replace line breaks with spaces
    string::size_type idx = 0;
    while((idx = s.find("\n", idx)) != string::npos)
    {
        s.insert(idx + 1, "  ");
        ++idx;
    }
}

void
TracerI::recalcMarginals()
{
    // pre-calculate marginals: accross trace types
    for ( int i=0; i<TracerI::AnyTrace; ++i ) {
        config_.verbosity[i][TracerI::ToAny] = 0;
        for ( int j=0; j<TracerI::ToAny-1; ++j ) {
            config_.verbosity[i][TracerI::ToAny] = MAX ( config_.verbosity[i][TracerI::ToAny], config_.verbosity[i][j] );
        }
    }
    // pre-calculate marginals: accross destination types
    for ( int j=0; j<TracerI::ToAny; ++j ) {
        config_.verbosity[TracerI::AnyTrace][j] = 0;
        for ( int i=0; i<TracerI::AnyTrace; ++i ) {
            config_.verbosity[TracerI::AnyTrace][j] = MAX ( config_.verbosity[TracerI::AnyTrace][j], config_.verbosity[i][j] );
        }
    }
}
