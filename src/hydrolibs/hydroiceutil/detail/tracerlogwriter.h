#ifndef HYDROICEUTIL_TRACERLOGWRITER_H
#define HYDROICEUTIL_TRACERLOGWRITER_H

#include <hydroiceutil/detail/syslogger.h>
#include <hydroiceutil/detail/logcompressor.h>
#include <map>

namespace hydroiceutil {
namespace detail {

//
// @brief Handles writing tracer information to a log (e.g. syslog in UNIX).
//
//        Does some compression: compresses repeated messages into
//        "Last message repeated XXX times" messages.  syslogd does
//        this for us anyway, but it can't handle the case where
//        multiple components interleave repeated messages.  Also,
//        within a single component we have to deal with the case
//        where multiple subsystems interleave repeated messages.
//        This class is designed to handle these cases.
//
// @author Alex Brooks
//
class TracerLogWriter
{
public: 

    TracerLogWriter( const std::string &compName, bool isApp );

    void logDebug(   const std::string &msg, const std::string &subsystem="" ) { log(SysLogger::Debug,msg,subsystem); }
    void logInfo(    const std::string &msg, const std::string &subsystem="" ) { log(SysLogger::Info,msg,subsystem); }
    void logNotice(  const std::string &msg, const std::string &subsystem="" ) { log(SysLogger::Notice,msg,subsystem); }
    void logWarning( const std::string &msg, const std::string &subsystem="" ) { log(SysLogger::Warning,msg,subsystem); }
    void logError(   const std::string &msg, const std::string &subsystem="" ) { log(SysLogger::Error,msg,subsystem); }

private: 

    void log( SysLogger::LogPriority  priority,
              const std::string      &msg,
              const std::string      &subsystem = "" );

    SysLogger sysLogger_;

    // Indexed by subsystem
    std::map<std::string,LogCompressor> logCompressors_;
};

}
}

#endif
