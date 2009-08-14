/*
 * Orca-Robotics Project: Components for robotics 
             http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_SIMPLE_TRACER_H
#define HYDROICEUTIL_SIMPLE_TRACER_H

#include <gbxutilacfr/tracer.h>
#include <IceUtil/Mutex.h>

namespace hydroiceutil
{

//! 
//! @brief Simple local tracing.
//!
//! @see LocalTracer
class SimpleTracer : public gbxutilacfr::Tracer
{
public:
    //! Constructor. System name is prepended to all tracing messages.
    SimpleTracer( const std::string& sysName="Tracer" );

    //! Prints out verbatim to stdout.
    virtual void print( const std::string &message );
    //! Prints out verbatim to stdout.
    virtual void info( const std::string &message, int level=1, bool localOnly=false )
        { info("",message,level,localOnly); }
    //! Prints out verbatim to stdout.
    virtual void warning( const std::string &message, int level=1, bool localOnly=false )
        { warning("",message,level,localOnly); }
    //! Prints out verbatim to stdout.
    virtual void error( const std::string &message, int level=1, bool localOnly=false )
        { error("",message,level,localOnly); }
    //! Prints out verbatim to stdout.
    virtual void debug( const std::string &message, int level=1, bool localOnly=false )
        { debug("",message,level,localOnly); }
    //! Always returns 10.
    virtual int verbosity( gbxutilacfr::TraceType traceType, gbxutilacfr::DestinationType destType ) const;

    virtual void info( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );
    virtual void warning( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );
    virtual void error( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );
    virtual void debug( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );

private:
    // a string to prepend to all trace messages    
    std::string prefix_;

    // custom outgoing commands
    void toDisplay( gbxutilacfr::TraceType traceType,
                    const std::string& message,
                    int level,
                    const std::string &subsystem );

    // utilities
    std::string assembleMessage( gbxutilacfr::TraceType  traceType,
                                 const std::string      &message,
                                 const std::string      &subsystem,
                                 int                     level );

    // Tracer may be called from multiple threads.
    IceUtil::Mutex mutex_;
};

} // namespace

#endif
