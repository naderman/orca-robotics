/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "syslogger.h"
#include <iostream>

#ifndef WIN32
#include <syslog.h>
#endif

using namespace std;

namespace orcaice {
    namespace detail {

#ifndef WIN32

        //
        // UNIX Version
        //

        SysLogger::SysLogger( const orcaice::Context &context )
            : compName_(context.name().component),
              isApp_(context.isApplication())
        {
            // cout<<"TRACE(syslogger.cpp): constructor()..." << endl;

            // Set up logging
            if ( context.isApplication() )
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
        SysLogger::log( int                priority,
                        const std::string &priorityString,
                        const std::string &msg )
        {
            if ( isApp_ )
                syslog( priority, "%s: %s", priorityString.c_str(), msg.c_str() );
            else
                syslog( priority, "%s: %s: %s", compName_.c_str(), priorityString.c_str(), msg.c_str() );
        }

        void
        SysLogger::logDebug(   const std::string &msg )
        {
            log( LOG_DEBUG, "DEBUG", msg );
        }
        void
        SysLogger::logInfo(    const std::string &msg )
        {
            log( LOG_INFO, "INFO", msg );
        }
        void
        SysLogger::logNotice(  const std::string &msg )
        {
            log( LOG_NOTICE, "NOTICE", msg );
        }
        void
        SysLogger::logWarning( const std::string &msg )
        {
            log( LOG_WARNING, "WARNING", msg );
        }
        void
        SysLogger::logError(   const std::string &msg )
        {
            log( LOG_ERR, "ERROR", msg );
        }

#else

        //
        // Windows Version
        //
        SysLogger::SysLogger( const orcaice::Context &context )
            : compName_(context.name().component),
              isApp_(context.isApplication())
        {
            cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
        }

        SysLogger::~SysLogger()
        {
        }

        void
        SysLogger::logDebug(   const std::string &msg )
        {
            cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
        }
        void
        SysLogger::logInfo(    const std::string &msg )
        {
            cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
        }
        void
        SysLogger::logNotice(  const std::string &msg )
        {
            cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
        }
        void
        SysLogger::logWarning( const std::string &msg )
        {
            cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
        }
        void
        SysLogger::logError(   const std::string &msg )
        {
            cout << "ERROR(syslogger.cpp): not implemented for Win32" << endl;
        }

#endif

    } // end namespace detail
} // end namespace orcaice
