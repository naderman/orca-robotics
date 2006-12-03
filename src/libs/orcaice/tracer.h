/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_TRACER_H
#define ORCAICE_TRACER_H

namespace orcaice
{

/*!
 *  @brief Local and remote tracing.
 *
 *  Orca Tracer is similar to the Ice Logger interface. We call it Tracer because
 *  we use it to log trace statements, warnings, error messages, etc (not data).
 *
 *  A single Tracer object is meant to be shared by all threads in the component so the
 *  implementation must be thread-safe.
 *
 *  Use heartbeat( status() ) to send the current status with every heartbeat message.
 *
 */
// implem notes:
//   - This class could also be defined as a local interface in Slice
//   - status() may be separated into a local Status interface.
// see also: StatusTracerI
class Tracer
{
public:

    //! Types of traced information
    enum TraceType {
        //! Information
        InfoTrace=0,
        //! Heartbeat
        HeartbeatTrace,
        //! Warning
        WarningTrace,
        //! Error
        ErrorTrace,
        //! Debug statement
        DebugTrace,
        //! Status statement
        StatusTrace,
        //! Other
        OtherTrace,
        //! Number of trace types
        NumberOfTraceTypes
    };

    //! Types of destinations for traced information.
    enum DestinationType {
        //! Write to stardard display
        ToDisplay=0,
        //! Push to an IceStorm topic, for remote monitoring or logging.
        ToTopic,
        //! Write to a file (currently not implemented)
        ToFile,
        //! Write to SysLog on Unix, EventLog on windows (currently only SysLog implemented)
        ToLog,
        //! Number of destination types
        NumberOfDestinationTypes
    };

    struct Config
    {
        // array of verbosity levels
        int verbosity[NumberOfTraceTypes][NumberOfDestinationTypes];
        // If the message and source are identical, ignore this message
        bool ignoreRepeatedWarnings;
        // If the message and source are identical, ignore this message
        bool ignoreRepeatedErrors;
        // affects only the printout to stdout. Remote messages always have a timestamp.
        bool doTimestamp;
    };
    

    virtual ~Tracer() {};

    
    //! Prints out verbatim to stdout.
    //! @see info
    virtual void print( const std::string &message ) = 0;

    //! Routing is determined by OtherToXXX parameter.
    virtual void trace( const std::string &category, const std::string &message, int level=1 ) = 0;

    //! Routing is determined by InfoToXxx parameter.
    virtual void info( const std::string &message, int level=1 ) = 0;

    //! Routing is determined by HeartbeatToXxx parameter.
    virtual void heartbeat( const std::string &message, int level=1 ) = 0;
    
    //! Routing is determined by WarningToXxx parameter.
    virtual void warning( const std::string &message, int level=1 ) = 0;
    
    //! Routing is determined by ErrorToXxx parameter.
    virtual void error( const std::string &message, int level=1 ) = 0;

    //! Routing is determined by DebugToXxx parameter.
    virtual void debug( const std::string &message, int level=1 ) = 0;

    //! Sets internal status to @p message so it can be returned when requested from the outside.
    //! If the status has changed it will be distributed to destinations according to
    //! StatusToXxx parameter. If @p force is TRUE, the new status is distributed regardless
    //! of whether there was a change or not (this is usefull for heartbeat functionality).
    virtual void status( const std::string &message, bool force=false ) = 0;

    //! Returns current status.
    virtual std::string status() const = 0;
};

} // namespace

#endif
