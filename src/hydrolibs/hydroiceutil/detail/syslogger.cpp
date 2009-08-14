/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "syslogger.h"
#include <iostream>
#include <gbxutilacfr/exceptions.h>
#include <sstream>

#ifndef WIN32
#   include <syslog.h>
#endif

using namespace std;

namespace hydroiceutil {
namespace detail {

std::string toPriorityString( SysLogger::LogPriority priority )
{
    switch ( priority )
    {
    case SysLogger::Debug:   return "DEBUG";
    case SysLogger::Info:    return "INFO";
    case SysLogger::Notice:  return "NOTICE";
    case SysLogger::Warning: return "WARNING";
    case SysLogger::Error:   return "ERROR";
    default:
    {
        stringstream ss;
        ss << "Unknown priority: " << (int)priority;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    }
}

#ifndef WIN32

//
// UNIX Version
//

namespace {

    int toUnixPriority( SysLogger::LogPriority priority )
    {
        switch ( priority )
        {
        case SysLogger::Debug:   return LOG_DEBUG;
        case SysLogger::Info:    return LOG_INFO;
        case SysLogger::Notice:  return LOG_NOTICE;
        case SysLogger::Warning: return LOG_WARNING;
        case SysLogger::Error:   return LOG_ERR;
        default:
        {
            stringstream ss;
            ss << "Unknown priority: " << (int)priority;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        }
    }
}

//////////////////////////////////////////////////////////////////////

SysLogger::SysLogger( const std::string &compName, bool isApp )
    : compName_(compName),
        isApp_(isApp)
{
    // cout<<"TRACE(syslogger.cpp): constructor()..." << endl;

    // Set up logging
    if ( isApp_ )
    {
        // Open it with our component name
        openlog(compName_.c_str(),LOG_PID,LOG_USER);
    }
    else
    {
        // We must be in an icebox.
        // The log may already be open, but we don't know so
        // open it again anyway.
        openlog("icebox",LOG_PID,LOG_USER);
    }
}

SysLogger::~SysLogger()
{
    closelog();
}

void 
SysLogger::log( LogPriority        priority,
                const std::string &msg )
{
    if ( isApp_ )
    {
        syslog( toUnixPriority(priority), "%s", msg.c_str() );
    }
    else
        syslog( toUnixPriority(priority), "%s: %s", compName_.c_str(), msg.c_str() );
}

#else

//
// Windows Version
//
SysLogger::SysLogger( const std::string &compName, bool isApp )
    : compName_(compName),
        isApp_(isApp)
{
    cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
}

SysLogger::~SysLogger()
{
}

void 
SysLogger::log( LogPriority        priority,
                const std::string &msg )
{
    cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
}

#endif

} // end namespace detail
} // end namespace orcaice
