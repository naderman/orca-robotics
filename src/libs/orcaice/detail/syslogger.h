/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_SYSLOGGER_H
#define ORCAICE_SYSLOGGER_H

#include <string>
#include <orcaice/context.h>

namespace orcaice {

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

            SysLogger( const orcaice::Context &context );
            ~SysLogger();

            void logDebug(   const std::string &msg );
            void logInfo(    const std::string &msg );
            void logNotice(  const std::string &msg );
            void logWarning( const std::string &msg );
            void logError(   const std::string &msg );
            
        private: 

            void log( int                priority,
                      const std::string &priorityString,
                      const std::string &msg );

            const std::string compName_;
            const bool        isApp_;
        };

    }

}

#endif
