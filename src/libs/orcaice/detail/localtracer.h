/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_LOCAL_TRACER_H
#define ORCAICE_LOCAL_TRACER_H

#include <orcaice/tracer.h>

#include <IceUtil/Mutex.h>

namespace orcaice
{
namespace detail
{

class SysLogger;

class LocalTracer : public orcaice::Tracer
{
public:
    LocalTracer( const orcaice::Context& context );
    virtual ~LocalTracer(); 

    // orcaice::Tracer interface
    
    virtual void print( const std::string &message );

    virtual void info( const std::string &message, int level=1 );
    
    virtual void warning( const std::string &message, int level=1 );
    
    virtual void error( const std::string &message, int level=1 );

    virtual void debug( const std::string &message, int level=1 );

    virtual int verbosity( TraceType traceType, DestinationType destType ) const;

protected:

    // Not implemented; prevents accidental use.
    LocalTracer( const LocalTracer& );
    LocalTracer& operator= ( const LocalTracer& );

    Tracer::Config  config_;
    orcaice::Context context_;
    std::string prefix_;

    // custom outgoing commands
    void toDisplay( const std::string& category, const std::string& message, int level );

    // to file
    void toFile( const std::string& category, const std::string& message, int level );
    std::ofstream *file_;

    // to log
    SysLogger *sysLogger_;

    std::string prevWarning_;
    std::string prevError_;

    // We only have one communicator but may have multiple threads.
    IceUtil::Mutex mutex_;
    
    // utilities
    void parseConfigFile();
    void assembleMessage( const std::string& category, const std::string& message, int level, std::string& s );
    void recalcMarginals();
};

} // namespace
} // namespace

#endif
