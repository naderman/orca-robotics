/*
 * Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
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
@brief Local and remote tracing.

Orca Tracer is similar to the Ice Logger interface. We call it Tracer because
we use it to log trace statements, e.g. warnings, error messages, etc (not data).

A single Tracer object is meant to be shared by all threads in the component so the
implementation must be thread-safe.

When the tracing message is cheap to generate, simply call one of the tracing functions. The routing to destinations will be performed internally.

@verbatim
Tracer* tracer = context().tracer();
std::string s = "something is wrong";
tracer->error( s );
@endverbatim

If the tracing message is a result of an expensive operation, you many want to perform the tracing test yourself and then call the tracing function. You may test verbosity for specific TraceType / DestinationType combinations or use summary fields AnyTrace and ToAny.

@verbatim
Tracer* tracer = context().tracer();
if ( tracer->verbocity( orcaice::Tracer::ErrorTrace, orcaice::Tracer::ToAny ) ) {
    std::string s = expensiveOperation();
    tracer->error( s );
}
@endverbatim


@par Tracer Configuration

- @c  Orca.Tracer.RequireIceStorm (bool)
    - orcaice::Component sets up a tracer and tries to connect to an
      IceStorm server on the same host in order to publish component's
      status messages. This parameter determines what happens if no server
      is found. If set to 0, the startup continues with status messages not
      published remotely. If set to 1, the application exits.
    - Default: 0

- @c  Orca.Tracer.Filename (string)
    - The name of the output file to which trace statements are saved. Each component creates its own file. If you want several component to write trace files and they execute in the same directory you have to set this property to unique file names in the component config file.
    - Default: "orca_component_trace.txt"
    
- @c  Orca.Tracer.Timestamp (bool)
    - Print timestamp before all trace statements.
    - Default: 1

Enum orcaice::Tracer::TraceType defines types of traced information. Enum orcaice::Tracer::DestinationType defines possible tracer destinations are. Verbosity levels range from 0 (nothing) to 10 (everything). The built-in defaults as follows:
@verbatim
                ToDisplay   ToNetwork   ToLog   ToFile
Info                1           0         0       0
Warning             1           0         0       0
Error              10           0         0       0
Debug               0           0         0       0
@endverbatim

A sample configuration file which sets all parameters to sensible defaults is shown here.

@include libs/orcaice/orcarc
 *
@see Status
 *
 */
// implem notes:
//   - The local API of this class could also be defined as a local interface in Slice.
class Tracer
{
public:
    virtual ~Tracer() {}; 

    //! Types of traced information
    enum TraceType {
        //! Information
        InfoTrace=0,
        //! Warning
        WarningTrace,
        //! Error
        ErrorTrace,
        //! Debug statement
        DebugTrace,
        //! Use this index to find out the maximum verbosity among all trace types to
        //! a particular destination.
        AnyTrace,
        //! Number of trace types
        NumberOfTraceTypes,
        //! Other (not used by orcaice tracers)
        OtherTrace
    };

    //! Types of destinations for traced information.
    enum DestinationType {
        //! Write to stardard display
        ToDisplay=0,
        //! Send over the network to an IceStorm topic
        ToNetwork,
        //! Write to SysLog on Unix, EventLog on windows (currently only SysLog implemented)
        ToLog,
        //! Write to a file
        ToFile,
        //! Use this index to request the maximum verbosity of a particular type among 
        //! all destinations
        ToAny,
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
        bool addTimestamp;
    };

    // LOCAL INTERFACE
    
    //! Prints out verbatim to stdout. It is never routed over the network.
    //! @see info
    virtual void print( const std::string &message ) = 0;

    //! Routing is determined by InfoToXxx parameter.
    virtual void info( const std::string &message, int level=1 ) = 0;
    
    //! Routing is determined by WarningToXxx parameter.
    virtual void warning( const std::string &message, int level=1 ) = 0;
    
    //! Routing is determined by ErrorToXxx parameter.
    virtual void error( const std::string &message, int level=1 ) = 0;

    //! Routing is determined by DebugToXxx parameter.
    virtual void debug( const std::string &message, int level=1 ) = 0;

    //! Returns the verbosity level for traceType to destType. This test is performed 
    //! internally by all tracing functions, e.g. error(). You may want to call this 
    //! function yourself @e before calling error() if there is a significant overhead
    //! in forming the tracing string. See class documentation for an example of such
    //! usage.
    virtual int verbosity( TraceType traceType, DestinationType destType ) const = 0;

    static std::string toString( Tracer::TraceType type )
    {
        switch ( type ) 
        {
        case Tracer::InfoTrace :
            return "info";
        case Tracer::WarningTrace :
            return "warn";
        case Tracer::ErrorTrace :
            return "error";
        case Tracer::DebugTrace :
            return "debug";
        default :
            return "other";
        }
    };

    static Tracer::TraceType toTraceType( const std::string& category )
    {
        if ( category=="info" )
            return Tracer::InfoTrace;
        else if ( category=="warn" )
            return Tracer::WarningTrace;
        else if ( category=="error" )
            return Tracer::ErrorTrace;
        else if ( category=="debug" )
            return Tracer::DebugTrace;
        else
            return Tracer::OtherTrace;
    };
};

} // namespace

#endif
