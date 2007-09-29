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

#include "simpletracer.h"

using namespace std;
using namespace orcaiceutil;

SimpleTracer::SimpleTracer( const std::string& prefix ) :
    prefix_(prefix)
{
}

void
SimpleTracer::print( const std::string &message )
{
    IceUtil::Mutex::Lock lock(mutex_);
    cout<<message<<endl;
}

void
SimpleTracer::info( const std::string &message, int level )
{
    toDisplay( "info", message, level );
}

void
SimpleTracer::warning( const std::string &message, int level )
{
    toDisplay( "warn", message, level );
}
    
void
SimpleTracer::error( const std::string &message, int level )
{
    toDisplay( "error", message, level );
}

void
SimpleTracer::debug( const std::string &message, int level )
{
    toDisplay( "debug", message, level );
}

int 
SimpleTracer::verbosity( TraceType traceType, DestinationType destType ) const
{
    return 10;
}

void
SimpleTracer::toDisplay( const std::string& category, const std::string& message, int level )
{
    string output;
    assembleMessage(category, message, level, output);
    
    // get a lock so that messages coming from different threads don't interfere with each other
    IceUtil::Mutex::Lock lock(mutex_);
    cout << output << endl;
}

void
SimpleTracer::assembleMessage( const std::string& category, const std::string& message, int level, std::string& s )
{
    s = "[ ";
    s += IceUtil::Time::now().toDateTime() + " ";
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
