/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <IceUtil/Time.h>
#include <gbxutilacfr/mathdefs.h>
#include <hydroutil/properties.h>

#include "localtracer.h"
#include "detail/tracerlogwriter.h"

using namespace std;
using namespace hydroiceutil;

LocalTracer::LocalTracer() : 
    file_(0),
    tracerLogWriter_(0)
{
    //
    // set defaults
    //
    setDefaults();
}

LocalTracer::LocalTracer( const hydroutil::Properties& props, const std::string& sysName ) :
    prefix_(sysName + ": "),
    properties_(props),
    file_(0),
    tracerLogWriter_(0)
{
    //
    // set defaults
    //
    setDefaults();

    //
    // parse properties
    //
    parseConfig();

    // decide if we need an output file.
    if ( config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToFile] ) {
        string filename = properties_.getPropertyWithDefault( "Filename", sysName+"_trace.txt" );
        file_ = new ofstream( filename.c_str() );
        if ( !file_->is_open() ) {
            cout<<"hydro::LocalTracer: Could not create file " << filename << endl;
        }
        else {
            cout<<"hydro::LocalTracer: Created output file " << filename << endl;
        }
    }
    
    // decide if we need a syslogger
    if ( config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToLog] ) {
        // can we get this info? do we want it?
        bool isApp = true;
        tracerLogWriter_ = new hydroiceutil::detail::TracerLogWriter( sysName, isApp );
        cout<<"hydro::LocalTracer: Created syslogger" << endl;
    }
}

LocalTracer::~LocalTracer()
{
    if ( tracerLogWriter_ ) delete tracerLogWriter_;
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

void
LocalTracer::print( const std::string &message )
{
    // not sure if we need it.
    // this is to serialize access to cout.
    IceUtil::Mutex::Lock lock(mutex_);

    cout<<message<<endl;
}

// NOTE: it'd be easier to just obtain a lock for the duration of this function but ...
// must be careful when callin toNetwork() which may perform remote calls with a mutex locked.
// It's safer not to do it.
void
LocalTracer::distribute( gbxutilacfr::TraceType  type,
                         const std::string      &message,
                         const std::string      &subsystem,
                         int                     level,
                         bool                    localOnly )
{
    int displayLevel, fileLevel, logLevel, networkLevel;
    {
        IceUtil::Mutex::Lock lock(mutex_);

        displayLevel = config_.verbosity[type][gbxutilacfr::ToDisplay];
        fileLevel = config_.verbosity[type][gbxutilacfr::ToFile];
        logLevel = config_.verbosity[type][gbxutilacfr::ToLog];
        networkLevel = config_.verbosity[type][gbxutilacfr::ToNetwork];
    }

    if ( displayLevel >= level )
        toDisplay( type, message, level, subsystem );

    if ( fileLevel >= level ) 
        toFile( type, message, level, subsystem );
    
    if ( logLevel >= level )
        toLog( type, message, level, subsystem );
    
    if ( !localOnly && networkLevel >= level )
        toNetwork( type, message, level, subsystem );
}

void
LocalTracer::info( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    assert( !message.empty() && "null message" );
    distribute( gbxutilacfr::InfoTrace, message, subsystem, level, localOnly );
}

void
LocalTracer::warning( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    assert( !message.empty() && "null message" );
    distribute( gbxutilacfr::WarningTrace, message, subsystem, level, localOnly );
}
    
void
LocalTracer::error( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    assert( !message.empty() && "null message" );
    distribute( gbxutilacfr::ErrorTrace, message, subsystem, level, localOnly );
}

void
LocalTracer::debug( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    assert( !message.empty() && "null message" );
    distribute( gbxutilacfr::DebugTrace, message, subsystem, level, localOnly );
}

int 
LocalTracer::verbosity( gbxutilacfr::TraceType traceType, gbxutilacfr::DestinationType destType ) const
{
    IceUtil::Mutex::Lock lock(mutex_);

    return config_.verbosity[traceType][destType];
}

void
LocalTracer::toDisplay( gbxutilacfr::TraceType  type,
                        const std::string      &message,
                        int                     level,
                        const std::string      &subsystem )
{
    string output = assembleDisplayOrFileMessage(type, message, subsystem, level );
    
    // not sure if we need it.
    // this is to serialize access to cout.
    IceUtil::Mutex::Lock lock(mutex_);

    cout << output << endl;
}

void
LocalTracer::toFile( gbxutilacfr::TraceType  type,
                     const std::string      &message,
                     int                     level,
                     const std::string      &subsystem )
{
    string output = assembleDisplayOrFileMessage(type, message, subsystem, level );
    
    // not sure if we need it.
    // this is to serialize access to the file.
    IceUtil::Mutex::Lock lock(mutex_);

    (*file_) << output << endl;
}

void
LocalTracer::toLog( gbxutilacfr::TraceType  type,
                    const std::string      &message,
                    int                     level,
                    const std::string      &subsystem )
{
    switch ( type )
    {
    case gbxutilacfr::InfoTrace :
        tracerLogWriter_->logInfo( message, subsystem );
        break;
    case gbxutilacfr::WarningTrace :
        tracerLogWriter_->logWarning( message, subsystem );
        break;
    case gbxutilacfr::ErrorTrace :
        tracerLogWriter_->logError( message, subsystem );
        break;
    case gbxutilacfr::DebugTrace :
        tracerLogWriter_->logDebug( message, subsystem );
        break;
    default :
        // this is an internal function, cannot be called with other types
        assert( !"Unsupported trace type in LocalTracer::toLog()" );
    }      
}

/////////// non-thread-safe utilities //////////////////

void
LocalTracer::setDefaults()
{
    //                 ToDisplay   ToNetwork   ToLog   ToFile
    // Info                1           0         0       0
    // Warning             1           0         0       0
    // Error              10           0         0       0
    // Debug               0           0         0       0
    //
    for ( int i=0; i<gbxutilacfr::NumberOfTraceTypes; ++i ) {
        for ( int j=0; j<gbxutilacfr::NumberOfDestinationTypes; ++j ) {
            config_.verbosity[i][j] = 0;
        }
    }
    config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToDisplay] = 1;
    config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToDisplay] = 1;
    config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToDisplay] = 10;
    config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToDisplay] = 0;

    config_.addTimestamp = 1;
}

void
LocalTracer::parseConfig()
{
    // set tracer configuration parameters
    // we assume that defaults were already set on all tracer properties
    
    // destination (defaults are zeros)
    properties_.getPropertyAsInt("InfoToDisplay",   config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToDisplay] );
    properties_.getPropertyAsInt("InfoToNetwork",   config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToNetwork] );
    properties_.getPropertyAsInt("InfoToLog",       config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToLog] );
    properties_.getPropertyAsInt("InfoToFile",      config_.verbosity[gbxutilacfr::InfoTrace][gbxutilacfr::ToFile] );

    properties_.getPropertyAsInt("WarningToDisplay",config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToDisplay] );
    properties_.getPropertyAsInt("WarningToNetwork",config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToNetwork] );
    properties_.getPropertyAsInt("WarningToLog",    config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToLog] );
    properties_.getPropertyAsInt("WarningToFile",   config_.verbosity[gbxutilacfr::WarningTrace][gbxutilacfr::ToFile] );

    properties_.getPropertyAsInt("ErrorToDisplay",  config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToDisplay] );
    properties_.getPropertyAsInt("ErrorToNetwork",  config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToNetwork] );
    properties_.getPropertyAsInt("ErrorToLog",      config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToLog] );
    properties_.getPropertyAsInt("ErrorToFile",     config_.verbosity[gbxutilacfr::ErrorTrace][gbxutilacfr::ToFile] );

    properties_.getPropertyAsInt("DebugToDisplay",  config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToDisplay] );
    properties_.getPropertyAsInt("DebugToNetwork",  config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToNetwork] );
    properties_.getPropertyAsInt("DebugToLog",      config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToLog] );
    properties_.getPropertyAsInt("DebugToFile",     config_.verbosity[gbxutilacfr::DebugTrace][gbxutilacfr::ToFile] );

    // pre-calculate marginals: accross trace types
    recalcMarginals();

    // format
    config_.addTimestamp = properties_.getPropertyAsIntWithDefault("Timestamp", 1 );

    // filtering
//     int dummy;
//     properties_.getPropertyAsIntWithDefault("IgnoreRepeatedWarnings",  dummy );
//     config_.ignoreRepeatedWarnings = dummy;
//     properties_.getPropertyAsInt("IgnoreRepeatedErrors",    dummy );
//     config_.ignoreRepeatedErrors = dummy;
}

std::string
LocalTracer::assembleDisplayOrFileMessage( gbxutilacfr::TraceType  type,
                                           const std::string      &message,
                                           const std::string      &subsystem,
                                           int                     level )
{
    std::string output = "[ ";
    if ( subsystem != "" )
    {
        output += subsystem + ": ";
    }
    if( config_.addTimestamp )
    {
        output += IceUtil::Time::now().toDateTime() + " ";
    }
    output += prefix_;
    // todo: would be nice to show level here, e.g. debug3:
    // but seems like an overkill to deal with osstreams
    output += " " + gbxutilacfr::toString(type) + ": ";
    output += message + " ]";

    // replace line breaks with spaces
    string::size_type idx = 0;
    while((idx = output.find("\n", idx)) != string::npos)
    {
        output.insert(idx + 1, "  ");
        ++idx;
    }
    return output;
}

void
LocalTracer::recalcMarginals()
{
    // pre-calculate marginals: accross trace types
    for ( int i=0; i<gbxutilacfr::AnyTrace; ++i ) {
        config_.verbosity[i][gbxutilacfr::ToAny] = 0;
        for ( int j=0; j<gbxutilacfr::ToAny; ++j ) {
            config_.verbosity[i][gbxutilacfr::ToAny] = MAX ( config_.verbosity[i][gbxutilacfr::ToAny], config_.verbosity[i][j] );
        }
    }
    // pre-calculate marginals: accross destination types
    for ( int j=0; j<gbxutilacfr::ToAny; ++j ) {
        config_.verbosity[gbxutilacfr::AnyTrace][j] = 0;
        for ( int i=0; i<gbxutilacfr::AnyTrace; ++i ) {
            config_.verbosity[gbxutilacfr::AnyTrace][j] = MAX ( config_.verbosity[gbxutilacfr::AnyTrace][j], config_.verbosity[i][j] );
        }
    }
    // pre-calculate total marginal: accross trace and destination types
    config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToAny] = 0;
    for ( int i=0; i<gbxutilacfr::AnyTrace; ++i ) {
        for ( int j=0; j<gbxutilacfr::ToAny; ++j ) {
            config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToAny] 
                = MAX ( config_.verbosity[gbxutilacfr::AnyTrace][gbxutilacfr::ToAny], config_.verbosity[i][j] );
        }
    }
}

/////////////// experimental //////////////

void 
LocalTracer::setSubsystemDebugLevel( const std::string &subsystem, int level )
{
    IceUtil::Mutex::Lock lock(mutex_);

    subsysVerbosity_[subsystem] = level;
}

void 
LocalTracer::subsystemDebug( const std::string &subsystem, const std::string &message, int level )
{
    IceUtil::Mutex::Lock lock(mutex_);

    std::map<std::string,int>::const_iterator it = subsysVerbosity_.find( subsystem );

    // if it's not set explicitely, default = 0, i.e. no tracing
    if ( it != subsysVerbosity_.end() ) {
        if ( it->second >= level )
            toDisplay( gbxutilacfr::DebugTrace, message, level, subsystem );
    }
}
