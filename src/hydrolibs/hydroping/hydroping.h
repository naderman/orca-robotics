#ifndef HYDROPING_HYDROPING_H
#define HYDROPING_HYDROPING_H

#include <hydroping/pinger.h>

namespace hydroping {

//!
//! Runs the 'hydroping' stand-alone executable (via the OS), returns the result.
//! Note that 'hydroping' needs to be run with root permissions, if you don't have them
//! setting 'runWithSudo' will prefix the command with 'sudo <cmd>'
//!
//! pathToHydroPing specifies the full path to the 'hydroping' binary, e.g. /usr/local/bin/hydroping
//!
//! Returns:
//! - >=0 : a latency in milliseconds
//! - <0  : ping timed out.
//!
//! Throws PingException if 'hydroping' ran, e.g. when DNS lookup of the host failed.
//! Throws gbxutilacfr::Exception if the system was unable to run 'hydroping', e.g. the
//! specified path to the binary was invalid or the permissions were insufficient.
double pingLatencyMs( const std::string &host,
                      bool               runWithSudo,
                      const std::string &pathToHydroPing );

}

#endif
