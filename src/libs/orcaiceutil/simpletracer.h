/*
 * Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_SIMPLE_TRACER_H
#define ORCAICEUTIL_SIMPLE_TRACER_H

#include <orcaiceutil/tracer.h>
#include <IceUtil/Mutex.h>

namespace orcaiceutil
{

//! 
//! @brief Simple local tracing.
//!
class SimpleTracer : public Tracer
{
public:
    //! Constructor. May specify a string to pre-pend to all trace
    //! messages, default ("tracer: ").
    SimpleTracer( const std::string& prefix="tracer: " );

    //! Prints out verbatim to stdout.
    virtual void print( const std::string &message );

    //! Prints out verbatim to stdout.
    virtual void info( const std::string &message, int level=1 );
    
    //! Prints out verbatim to stdout.
    virtual void warning( const std::string &message, int level=1 );
    
    //! Prints out verbatim to stdout.
    virtual void error( const std::string &message, int level=1 );

    //! Prints out verbatim to stdout.
    virtual void debug( const std::string &message, int level=1 );

    //! Always returns 10.
    virtual int verbosity( TraceType traceType, DestinationType destType ) const;

private:
    // a string to prepend to all trace messages    
    std::string prefix_;

    // custom outgoing commands
    void toDisplay( const std::string& category, const std::string& message, int level );

    // utilities
    void assembleMessage( const std::string& category, const std::string& message, int level, std::string& s );

    // Tracer may be called from multiple threads.
    IceUtil::Mutex mutex_;
};

} // namespace

#endif
