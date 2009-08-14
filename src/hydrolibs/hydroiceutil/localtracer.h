/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_LOCAL_TRACER_H
#define HYDROICEUTIL_LOCAL_TRACER_H

#include <gbxutilacfr/tracer.h>
#include <hydroutil/properties.h>
#include <hydroutil/uncopyable.h>

#include <map>
#include <IceUtil/Mutex.h>

namespace hydroiceutil
{

namespace detail {
    class TracerLogWriter;
}

/*! 
@brief Full-featured local tracing.

@see SimpleTracer
*/
class LocalTracer : public gbxutilacfr::Tracer,
                    public hydroutil::Uncopyable
{
public:
    //! Constructor
    LocalTracer();
    //! Constructor with properties and system name. System name is prepended to all tracing messages.
    LocalTracer( const hydroutil::Properties& props, const std::string& sysName="Tracer" );
    virtual ~LocalTracer(); 

    // from gbxutilacfr::Tracer
    virtual void print( const std::string &message );
    virtual void info( const std::string &message, int level=1, bool localOnly=false )
        { info("",message,level,localOnly); }
    virtual void warning( const std::string &message, int level=1, bool localOnly=false )
        { warning("",message,level,localOnly); }
    virtual void error( const std::string &message, int level=1, bool localOnly=false )
        { error("",message,level,localOnly); }
    virtual void debug( const std::string &message, int level=1, bool localOnly=false )
        { debug("",message,level,localOnly); }
    virtual int verbosity( gbxutilacfr::TraceType traceType, gbxutilacfr::DestinationType destType ) const;

    virtual void info( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );
    virtual void warning( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );
    virtual void error( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );
    virtual void debug( const std::string &subsystem, const std::string &message, int level=1, bool localOnly=false );

    // experimental!
    virtual void setSubsystemDebugLevel( const std::string &subsystem, int level=0 );
    virtual void subsystemDebug( const std::string &subsystem, const std::string &message, int level=1 );

protected:

    std::string prefix_;

    // locking config_
    IceUtil::Mutex mutex_;
    Tracer::Config  config_;

    // recalculates "marginal" verbosity levels, i.e. [...][ToAny] and [AnyTrace][...]
    // not thread safe.
    void recalcMarginals();

    // destination functions
    void toDisplay( gbxutilacfr::TraceType  type,
                    const std::string      &message,
                    int                     level,
                    const std::string      &subsystem="" );
    void toFile( gbxutilacfr::TraceType  type,
                 const std::string      &message,
                 int                     level,
                 const std::string      &subsystem="" );
    void toLog( gbxutilacfr::TraceType  type,
                const std::string      &message,
                int                     level, 
                const std::string      &subsystem="" );
    //! This event is called when there's a trace message destined for publication over the network.
    //! This implementation does nothing. Reimplement this function to do something useful.
    virtual void toNetwork( gbxutilacfr::TraceType  type,
                            const std::string      &message,
                            int                     level,
                            const std::string      &subsytem = "" ) {};

private:

    // this is the workhorse which actually distributes traces to destinations.
    // thread safe.
    void distribute( gbxutilacfr::TraceType  type,
                     const std::string      &message,
                     const std::string      &subsystem,
                     int                     level,
                     bool                    localOnly = false );
    
    // utilities, not thread-safe!
    void setDefaults();
    void parseConfig();
    std::string assembleDisplayOrFileMessage( gbxutilacfr::TraceType  type,
                                              const std::string      &message,
                                              const std::string      &subsystem,
                                              int                     level );

    hydroutil::Properties properties_;
    std::ofstream *file_;
    detail::TracerLogWriter *tracerLogWriter_;
    
    // experimental
    std::map<std::string,int> subsysVerbosity_;
};

} // namespace

#endif
