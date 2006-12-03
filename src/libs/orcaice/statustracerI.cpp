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

#include "statustracerI.h"
#include "syslogger.h"

using namespace std;

namespace orcaice
{

StatusTracerI::StatusTracerI( const orcaice::Context & context )
          : context_(context),
            compName_(context.name()),
            file_(NULL),
            sysLogger_(NULL),
            statusTopic_(0),
            statusPublisher_(0)
{
    prefix_ = orcaice::toString(compName_) + ": ";
    parseConfigFile();
        
    // do we need IceStorm topic?
    bool needIceStorm = false;
    for ( int i=0; i<NumberOfTraceTypes; ++i ) {
        if ( config_.verbosity[i][ToTopic] )
        {
            // cout<<"TRACE(statustracerI.cpp): Need syslogger for traceType " << i << endl;
            needIceStorm = true;
            break;
        }
    }
    if ( needIceStorm ) {
        connectToIceStorm();
    }

    // do we need output file?
    bool needFile = false;
    for ( int i=0; i<NumberOfTraceTypes; ++i ) {
        if ( config_.verbosity[i][ToFile] )
        {
            needFile = true;
            break;
        }
    }
    if ( needFile ) {
        string filename = context_.properties()->getProperty("Orca.Tracer.Filename");
        file_ = new ofstream( filename.c_str() );
        if ( !file_->is_open() ) {
            initTracerError( "Could not create file " + filename );
        }
    }
    
    // do we need a syslogger?
    bool needSysLogger = false;
    for ( int i=0; i<NumberOfTraceTypes; ++i ) {
        if ( config_.verbosity[i][ToLog] )
        {
            // cout<<"TRACE(statustracerI.cpp): Need syslogger for traceType " << i << endl;
            needSysLogger = true;
            break;
        }
    }
    if ( needSysLogger ) {
        sysLogger_ = new orcaice::detail::SysLogger(context);
    }

    // populate the status data object
    orca::StatusDataPtr currentStatus = new orca::StatusData(
        orcaice::getNow(), compName_, "status", 1, "initialized" );
    // stick it into the data pipe
    statusPipe_.set( currentStatus );

//     cout<<"TRACE(statustracerI.cpp): verbosity:" << endl;
//     for ( int i=0; i < NumberOfTraceTypes; i++ )
//     {
//         cout << "  ";
//         for ( int j=0; j < NumberOfDestinationTypes; j++ )
//         {
//             cout << config_.verbosity[i][j] << " ";
//         }
//         cout << endl;
//     }
}

StatusTracerI::~StatusTracerI()
{
    if ( sysLogger_ ) delete sysLogger_;
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

void
StatusTracerI::parseConfigFile()
{
    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // set tracer configuration parameters
    // defaults were already set on all tracer properties
    
    // destination (defaults are zeros)
    config_.verbosity[Tracer::InfoTrace][Tracer::ToDisplay]     = props->getPropertyAsInt("Orca.Tracer.InfoToStdout");
    config_.verbosity[Tracer::InfoTrace][Tracer::ToFile]        = props->getPropertyAsInt("Orca.Tracer.InfoToFile");
    config_.verbosity[Tracer::InfoTrace][Tracer::ToTopic]       = props->getPropertyAsInt("Orca.Tracer.InfoToTopic");
    config_.verbosity[Tracer::InfoTrace][Tracer::ToLog]         = props->getPropertyAsInt("Orca.Tracer.InfoToLog");
    config_.verbosity[Tracer::HeartbeatTrace][Tracer::ToDisplay]= props->getPropertyAsInt("Orca.Tracer.HeartbeatToStdout");
    config_.verbosity[Tracer::HeartbeatTrace][Tracer::ToFile]   = props->getPropertyAsInt("Orca.Tracer.HeartbeatToFile");
    config_.verbosity[Tracer::HeartbeatTrace][Tracer::ToTopic]  = props->getPropertyAsInt("Orca.Tracer.HeartbeatToTopic");
    config_.verbosity[Tracer::HeartbeatTrace][Tracer::ToLog]    = props->getPropertyAsInt("Orca.Tracer.HeartbeatToLog");
    config_.verbosity[Tracer::WarningTrace][Tracer::ToDisplay]  = props->getPropertyAsInt("Orca.Tracer.WarningToStdout");
    config_.verbosity[Tracer::WarningTrace][Tracer::ToFile]     = props->getPropertyAsInt("Orca.Tracer.WarningToFile");
    config_.verbosity[Tracer::WarningTrace][Tracer::ToTopic]    = props->getPropertyAsInt("Orca.Tracer.WarningToTopic");
    config_.verbosity[Tracer::WarningTrace][Tracer::ToLog]      = props->getPropertyAsInt("Orca.Tracer.WarningToLog");
    config_.verbosity[Tracer::ErrorTrace][Tracer::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.ErrorToStdout");
    config_.verbosity[Tracer::ErrorTrace][Tracer::ToFile]       = props->getPropertyAsInt("Orca.Tracer.ErrorToFile");
    config_.verbosity[Tracer::ErrorTrace][Tracer::ToTopic]      = props->getPropertyAsInt("Orca.Tracer.ErrorToTopic");
    config_.verbosity[Tracer::ErrorTrace][Tracer::ToLog]        = props->getPropertyAsInt("Orca.Tracer.ErrorToLog");
    config_.verbosity[Tracer::DebugTrace][Tracer::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.DebugToStdout");
    config_.verbosity[Tracer::DebugTrace][Tracer::ToFile]       = props->getPropertyAsInt("Orca.Tracer.DebugToFile");
    config_.verbosity[Tracer::DebugTrace][Tracer::ToTopic]      = props->getPropertyAsInt("Orca.Tracer.DebugToTopic");
    config_.verbosity[Tracer::DebugTrace][Tracer::ToLog]        = props->getPropertyAsInt("Orca.Tracer.DebugToLog");
    config_.verbosity[Tracer::OtherTrace][Tracer::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.OtherToStdout");
    config_.verbosity[Tracer::OtherTrace][Tracer::ToFile]       = props->getPropertyAsInt("Orca.Tracer.OtherToFile");
    // remote routing of "other" traces must be disabled when standard Ice tracing is channeled through.
    config_.verbosity[Tracer::OtherTrace][Tracer::ToTopic]      = props->getPropertyAsInt("Orca.Tracer.OtherToTopic");
    config_.verbosity[Tracer::OtherTrace][Tracer::ToLog]        = props->getPropertyAsInt("Orca.Tracer.OtherToLog");

    config_.verbosity[Tracer::StatusTrace][Tracer::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.StatusToStdout");
    config_.verbosity[Tracer::StatusTrace][Tracer::ToFile]       = props->getPropertyAsInt("Orca.Tracer.StatusToFile");
    config_.verbosity[Tracer::StatusTrace][Tracer::ToTopic]      = props->getPropertyAsInt("Orca.Tracer.StatusToTopic");
    config_.verbosity[Tracer::StatusTrace][Tracer::ToLog]        = props->getPropertyAsInt("Orca.Tracer.StatusToLog");
    // filtering
    config_.ignoreRepeatedWarnings = props->getPropertyAsInt( "Orca.Tracer.IgnoreRepeatedWarnings" );
    config_.ignoreRepeatedErrors = props->getPropertyAsInt( "Orca.Tracer.IgnoreRepeatedErrors" );
    // format
    config_.doTimestamp        = props->getPropertyAsInt( "Orca.Tracer.Timestamp" );
}


void
StatusTracerI::icestormConnectFailed( const orca::FQTopicName &fqTName,
                                      orca::StatusConsumerPrx &statusPublisher,
                                      bool isStatusTopicRequired )
{
    if ( isStatusTopicRequired ) 
    {
        std::string s = prefix_+": Failed to connect to an IceStorm status topic '"+
            orcaice::toString( fqTName )+"'\n" +
            "\tYou may allow to proceed by setting Orca.Tracer.RequireStatusTopic=0.";
        initTracerError( s );
        // this should kill the app
        exit(1);
    }
    else 
    {
        statusPublisher = 0;
        std::string s = prefix_+": Failed to connect to an IceStorm status topic\n";
        s += "\tAll trace messages will be local.\n";
        s += "\tYou may enforce connection by setting Orca.Tracer.RequireStatusTopic=1.";
        initTracerWarning( s );

        // turn off all outputs toTopic
        for ( int i=0; i<NumberOfTraceTypes; ++i ) {
            config_.verbosity[i][ToTopic] = 0;
        }
        // move on
    }
}

bool
StatusTracerI::connectToIceStorm()
{
    statusTopic_     = 0;
    statusPublisher_ = 0;

    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // are we required to connect to status topic? (there's always default value for this property)
    bool isStatusTopicRequired = props->getPropertyAsInt( "Orca.Tracer.RequireStatusTopic" );

    orca::FQTopicName fqTName = orcaice::toStatusTopic( context_.name() );
    initTracerPrint( prefix_+": Connecting to status topic "+orcaice::toString( fqTName ));

    try
    {
        statusTopic_ = orcaice::connectToTopicWithString<orca::StatusConsumerPrx>(
            context_, statusPublisher_, orcaice::toString( fqTName ) );
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( prefix_+": Caught exception while connecting to IceStorm: "+e.what() );
        icestormConnectFailed( fqTName, statusPublisher_, isStatusTopicRequired );
    } // catch
    catch ( Ice::Exception &e )
    {
        std::stringstream s;
        s << prefix_ << ": Caught exception while connecting to IceStorm: " << e;
        initTracerError( s.str() );
        icestormConnectFailed( fqTName, statusPublisher_, isStatusTopicRequired );
    }
    catch ( ... )
    {
        initTracerError( prefix_+": Caught unknown exception while connecting to IceStorm." );
        icestormConnectFailed( fqTName, statusPublisher_, isStatusTopicRequired );
    }

    if ( statusPublisher_ ) {
        initTracerPrint( prefix_+": Tracer connected to "+orcaice::toString(fqTName) );
    }

    return statusPublisher_ != 0;
}

orca::StatusDataPtr
StatusTracerI::getData(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;
    orca::StatusDataPtr currentStatus;
    statusPipe_.get( currentStatus );
    // use the last status message, but update the time stamp.
    // does this make sense?
    orcaice::setToNow( currentStatus->timeStamp );
    
    return currentStatus;
}

void
StatusTracerI::subscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !statusTopic_ ) {
        throw orca::SubscriptionFailedException("Component does not have a topic to publish its status.");
    }
    
    //cout<<"subscription request"<<endl;
    IceStorm::QoS qos;    
    statusTopic_->subscribe( qos, subscriber );
}

void
StatusTracerI::unsubscribe(const ::orca::StatusConsumerPrx& subscriber, const ::Ice::Current&)
{
    if ( !statusTopic_ ) {
        return;
    }
    
    //cout<<"unsubscription request"<<endl;
    statusTopic_->unsubscribe( subscriber );
}

void
StatusTracerI::status( const std::string &message, bool force )
{
    // get the current status and update it
    orca::StatusDataPtr currentStatus;
    statusPipe_.get( currentStatus );

    if ( currentStatus->message != message )
    {
        currentStatus->message = message;
        orcaice::setToNow( currentStatus->timeStamp );
        // check the parameter?
        toTopic( "status", message, 1 );
    }
    else
    {
        // the message has not changed, just update the time stamp
        orcaice::setToNow( currentStatus->timeStamp );
    }

    // stick it back into the data pipe
    statusPipe_.set( currentStatus );
}

std::string
StatusTracerI::status() const
{
    orca::StatusDataPtr currentStatus;
    statusPipe_.get( currentStatus );
    
    return currentStatus->message;
}

void
StatusTracerI::print( const std::string &message )
{
    IceUtil::Mutex::Lock lock(mutex_);
    cout<<message<<endl;
}

void
StatusTracerI::trace( const std::string& category, const std::string& message, int level )
{
    if ( config_.verbosity[OtherTrace][ToDisplay] >= level ) {
        toDisplay( category, message, 1 );
    }
    // If TracerI implements Ice::Logger interface,
    // remote routing of "other" traces must be disabled to prevent lockup
    // when standard Ice tracing is turned on
    if ( config_.verbosity[OtherTrace][ToTopic] >= level ) {
        toTopic( category, message, 1 );
    }
    if ( config_.verbosity[OtherTrace][ToFile] >= level ) {
        toFile( category, message, 1 );
    }
    if ( config_.verbosity[OtherTrace][ToLog] >= level ) {
        sysLogger_->logDebug( message );
    }
}

void
StatusTracerI::info( const std::string &message, int level )
{
    if ( config_.verbosity[InfoTrace][ToDisplay] >= level ) {
        toDisplay( "info", message, level );
    }
    if ( config_.verbosity[InfoTrace][ToTopic] >= level ) {
        toTopic( "info", message, level );
    }
    if ( config_.verbosity[InfoTrace][ToFile] >= level ) {
        toFile( "info", message, level );
    }   
    if ( config_.verbosity[InfoTrace][ToLog] >= level ) {
        sysLogger_->logInfo( message );
    }   
}

// heartbeat-to-topic is hardwired to be ON.
void
StatusTracerI::heartbeat( const std::string &message, int level )
{
    if ( config_.verbosity[HeartbeatTrace][ToDisplay] >= 1 ) {
        toDisplay( "heartbeat", message, level );
    }
    //cout<<"finished heartbeat->display"<<endl;
    if ( config_.verbosity[HeartbeatTrace][ToTopic] >= 1 ) {
        toTopic( "heartbeat", message, level );
    }
    //cout<<"finished heartbeat->topic"<<endl;
    if ( config_.verbosity[HeartbeatTrace][ToFile] >= 1 ) {
        toFile( "heartbeat", message, level );
    }
    if ( config_.verbosity[HeartbeatTrace][ToLog] >= 1 ) {
        sysLogger_->logNotice( message );
    }
}

void
StatusTracerI::warning( const std::string &message, int level )
{
    if ( config_.verbosity[WarningTrace][ToDisplay] >= level ) {
        toDisplay( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToTopic] >= level ) {
        toTopic( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToFile] >= level ) {
        toFile( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToLog] >= level ) {
        sysLogger_->logWarning( message );
    }
}
    
void
StatusTracerI::error( const std::string &message, int level )
{
    if ( config_.verbosity[ErrorTrace][ToDisplay] >= level ) {
        toDisplay( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToTopic] >= level ) {
        toTopic( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToFile] >= level ) {
        toFile( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToLog] >= level ) {
        sysLogger_->logError( message );
    }
}

void
StatusTracerI::debug( const std::string &message, int level )
{
    if ( config_.verbosity[DebugTrace][ToDisplay] >= level ) {
        toDisplay( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToTopic] >= level ) {
        toTopic( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToFile] >= level ) {
        toFile( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToLog] >= level ) {
        sysLogger_->logDebug( message );
    }
}

void
StatusTracerI::assembleMessage( const std::string& category, const std::string& message, int level, std::string& s )
{
    s = "[ ";
    if(config_.doTimestamp)
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
StatusTracerI::toDisplay( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    cout << output << endl;
}

void
StatusTracerI::toTopic( const std::string& category, const std::string& message, int level )
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
        statusPublisher_->setData( statusData );
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

void
StatusTracerI::toFile( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    (*file_) << output << endl;
}

} // namespace
