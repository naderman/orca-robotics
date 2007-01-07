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

#include "localtracer.h"
#include "syslogger.h"

using namespace std;
using namespace orcaice::detail;

LocalTracer::LocalTracer( const orcaice::Context & context )
          : context_(context),
            file_(NULL),
            sysLogger_(NULL)
{
    prefix_ = orcaice::toString(context.name()) + ": ";
    parseConfigFile();

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

LocalTracer::~LocalTracer()
{
    if ( sysLogger_ ) delete sysLogger_;
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

void
LocalTracer::parseConfigFile()
{
    // get properties for our component
    Ice::PropertiesPtr props = context_.properties();

    // set tracer configuration parameters
    // defaults were already set on all tracer properties
    
    // destination (defaults are zeros)
    config_.verbosity[LocalTracer::InfoTrace][LocalTracer::ToDisplay]     = props->getPropertyAsInt("Orca.Tracer.InfoToDisplay");
    config_.verbosity[LocalTracer::InfoTrace][LocalTracer::ToNetwork]     = props->getPropertyAsInt("Orca.Tracer.InfoToNetwork");
    config_.verbosity[LocalTracer::InfoTrace][LocalTracer::ToLog]         = props->getPropertyAsInt("Orca.Tracer.InfoToLog");
    config_.verbosity[LocalTracer::InfoTrace][LocalTracer::ToFile]        = props->getPropertyAsInt("Orca.Tracer.InfoToFile");

    config_.verbosity[LocalTracer::WarningTrace][LocalTracer::ToDisplay]  = props->getPropertyAsInt("Orca.Tracer.WarningToDisplay");
    config_.verbosity[LocalTracer::WarningTrace][LocalTracer::ToNetwork]  = props->getPropertyAsInt("Orca.Tracer.WarningToNetwork");
    config_.verbosity[LocalTracer::WarningTrace][LocalTracer::ToLog]      = props->getPropertyAsInt("Orca.Tracer.WarningToLog");
    config_.verbosity[LocalTracer::WarningTrace][LocalTracer::ToFile]     = props->getPropertyAsInt("Orca.Tracer.WarningToFile");

    config_.verbosity[LocalTracer::ErrorTrace][LocalTracer::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.ErrorToDisplay");
    config_.verbosity[LocalTracer::ErrorTrace][LocalTracer::ToNetwork]    = props->getPropertyAsInt("Orca.Tracer.ErrorToNetwork");
    config_.verbosity[LocalTracer::ErrorTrace][LocalTracer::ToLog]        = props->getPropertyAsInt("Orca.Tracer.ErrorToLog");
    config_.verbosity[LocalTracer::ErrorTrace][LocalTracer::ToFile]       = props->getPropertyAsInt("Orca.Tracer.ErrorToFile");

    config_.verbosity[LocalTracer::DebugTrace][LocalTracer::ToDisplay]    = props->getPropertyAsInt("Orca.Tracer.DebugToDisplay");
    config_.verbosity[LocalTracer::DebugTrace][LocalTracer::ToNetwork]    = props->getPropertyAsInt("Orca.Tracer.DebugToNetwork");
    config_.verbosity[LocalTracer::DebugTrace][LocalTracer::ToLog]        = props->getPropertyAsInt("Orca.Tracer.DebugToLog");
    config_.verbosity[LocalTracer::DebugTrace][LocalTracer::ToFile]       = props->getPropertyAsInt("Orca.Tracer.DebugToFile");

    // pre-calculate marginals: accross trace types
    recalcMarginals();

    // filtering
    config_.ignoreRepeatedWarnings = props->getPropertyAsInt( "Orca.Tracer.IgnoreRepeatedWarnings" );
    config_.ignoreRepeatedErrors   = props->getPropertyAsInt( "Orca.Tracer.IgnoreRepeatedErrors" );
    // format
    config_.addTimestamp           = props->getPropertyAsInt( "Orca.Tracer.Timestamp" );
}

void
LocalTracer::print( const std::string &message )
{
    IceUtil::Mutex::Lock lock(mutex_);
    cout<<message<<endl;
}

void
LocalTracer::info( const std::string &message, int level )
{
    if ( config_.verbosity[InfoTrace][ToDisplay] >= level ) {
        toDisplay( "info", message, level );
    }
    if ( config_.verbosity[InfoTrace][ToFile] >= level ) {
        toFile( "info", message, level );
    }   
    if ( config_.verbosity[InfoTrace][ToLog] >= level ) {
        sysLogger_->logInfo( message );
    }   
}

void
LocalTracer::warning( const std::string &message, int level )
{
    if ( config_.verbosity[WarningTrace][ToDisplay] >= level ) {
        toDisplay( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToFile] >= level ) {
        toFile( "warn", message, level );
    }
    if ( config_.verbosity[WarningTrace][ToLog] >= level ) {
        sysLogger_->logWarning( message );
    }
}
    
void
LocalTracer::error( const std::string &message, int level )
{
    if ( config_.verbosity[ErrorTrace][ToDisplay] >= level ) {
        toDisplay( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToFile] >= level ) {
        toFile( "error", message, level );
    }
    if ( config_.verbosity[ErrorTrace][ToLog] >= level ) {
        sysLogger_->logError( message );
    }
}

void
LocalTracer::debug( const std::string &message, int level )
{
    if ( config_.verbosity[DebugTrace][ToDisplay] >= level ) {
        toDisplay( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToFile] >= level ) {
        toFile( "debug", message, level );
    }
    if ( config_.verbosity[DebugTrace][ToLog] >= level ) {
        sysLogger_->logDebug( message );
    }
}

int 
LocalTracer::verbosity( TraceType traceType, DestinationType destType ) const
{
    return config_.verbosity[traceType][destType];
}

void
LocalTracer::toDisplay( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    cout << output << endl;
}

void
LocalTracer::toFile( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    (*file_) << output << endl;
}

void
LocalTracer::assembleMessage( const std::string& category, const std::string& message, int level, std::string& s )
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
LocalTracer::recalcMarginals()
{
    // pre-calculate marginals: accross trace types
    for ( int i=0; i<LocalTracer::AnyTrace; ++i ) {
        config_.verbosity[i][LocalTracer::ToAny] = 0;
        for ( int j=0; j<LocalTracer::ToAny-1; ++j ) {
            config_.verbosity[i][LocalTracer::ToAny] = MAX ( config_.verbosity[i][LocalTracer::ToAny], config_.verbosity[i][j] );
        }
    }
    // pre-calculate marginals: accross destination types
    for ( int j=0; j<LocalTracer::ToAny; ++j ) {
        config_.verbosity[LocalTracer::AnyTrace][j] = 0;
        for ( int i=0; i<LocalTracer::AnyTrace; ++i ) {
            config_.verbosity[LocalTracer::AnyTrace][j] = MAX ( config_.verbosity[LocalTracer::AnyTrace][j], config_.verbosity[i][j] );
        }
    }
}
