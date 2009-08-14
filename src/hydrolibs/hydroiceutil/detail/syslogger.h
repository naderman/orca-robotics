/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_SYSLOGGER_H
#define HYDROICEUTIL_SYSLOGGER_H

#include <string>

namespace hydroiceutil {

namespace detail {

//
// Logs things to syslog under UNIX, or the Event Log under Windows.
// Currently, only the UNIX side is implemented.
//
// Not intended for use directly by libOrcaIce users, but rather internally in libOrcaIce.
//
// @author Alex Brooks
//
class SysLogger
{

public: 

    enum LogPriority {
        Debug,
        Info,
        Notice,
        Warning,
        Error,
    };

    SysLogger( const std::string &compName, bool isApp );
    ~SysLogger();

    void logDebug(   const std::string &msg ) { log( Debug, msg ); }
    void logInfo(    const std::string &msg ) { log( Info, msg ); }
    void logNotice(  const std::string &msg ) { log( Notice, msg ); }
    void logWarning( const std::string &msg ) { log( Warning, msg ); }
    void logError(   const std::string &msg ) { log( Error, msg ); }

    // Allow setting priority as a parameter
    void log( LogPriority priority, const std::string &msg );

private: 

    const std::string compName_;
    const bool        isApp_;
};

// Non-member functions
std::string toPriorityString( SysLogger::LogPriority priority );

} // namespace
} // namespace

#endif
