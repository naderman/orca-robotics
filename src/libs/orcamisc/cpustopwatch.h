#ifndef ORCAMISC_CPUSTOPWATCH_H
#define ORCAMISC_CPUSTOPWATCH_H

#include <orcamisc/stopwatch.h>

namespace orcamisc {

//!
//! @brief times user-space CPU time
//!
//! This fucker times _user-space_cpu_time_, not wallclock time.
//! This should give some idea of how hard your process is working, relatively
//! independent of system load or how much time is spent waiting for I/O.
//!
//! The 'start' and 'stop' buttons do the obvious thing.
//! 'startWithoutReset' starts the clock ticking _without_ resetting first.
//! 'reset' resets and stops the clock.
//!
//! @sa Stopwatch
//! @sa RealTimeStopwatch
//!
//! @author Alex Brooks
//!
class CpuStopwatch : public Stopwatch
{

public: 

    CpuStopwatch( bool startOnInitialisation=true );

private: 

    void getTimeNow( struct timeval &now ) const;
};

}

#endif
