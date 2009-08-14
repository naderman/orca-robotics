/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <IceUtil/Time.h>

#include "simpletracer.h"

using namespace std;
using namespace hydroiceutil;

SimpleTracer::SimpleTracer( const std::string& sysName ) :
    prefix_(sysName + ": ")
{
}

void
SimpleTracer::print( const std::string &message )
{
    IceUtil::Mutex::Lock lock(mutex_);
    cout<<message<<endl;
}

void
SimpleTracer::info( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    toDisplay( gbxutilacfr::InfoTrace, message, level, subsystem );
}

void
SimpleTracer::warning( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    toDisplay( gbxutilacfr::WarningTrace, message, level, subsystem );
}
    
void
SimpleTracer::error( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    toDisplay( gbxutilacfr::ErrorTrace, message, level, subsystem );
}

void
SimpleTracer::debug( const std::string &subsystem, const std::string &message, int level, bool localOnly )
{
    toDisplay( gbxutilacfr::DebugTrace, message, level, subsystem );
}

int 
SimpleTracer::verbosity( gbxutilacfr::TraceType traceType, gbxutilacfr::DestinationType destType ) const
{
    return 10;
}

void
SimpleTracer::toDisplay( gbxutilacfr::TraceType traceType, const std::string& message, int level, const std::string &subsystem )
{
    string output = assembleMessage(traceType, message, subsystem, level );
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    cout << output << endl;
}

std::string
SimpleTracer::assembleMessage( gbxutilacfr::TraceType  traceType,
                               const std::string      &message,
                               const std::string      &subsystem,
                               int                     level )
{
    std::string output = "[ ";
    if ( subsystem != "" )
        output += subsystem+": ";
    output += IceUtil::Time::now().toDateTime() + " ";
    output += prefix_;
    // todo: would be nice to show level here, e.g. debug3:
    // but seems like an overkill to deal with osstreams
    output += " " + gbxutilacfr::toString(traceType) + ": ";
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
