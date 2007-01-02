#ifndef ORCAMISC_REALTIMESTOPWATCH_H
#define ORCAMISC_REALTIMESTOPWATCH_H

#include <orcamisc/stopwatch.h>

namespace orcamisc {

//!
//! @brief times real time
//!
//! This fucker times wallclock time.
//!
//! The 'start' and 'stop' buttons do the obvious thing.
//! 'startWithoutReset' starts the clock ticking _without_ resetting first.
//! 'reset' resets and stops the clock.
//!
//! @sa CpuStopwatch
//!
//! @author Alex Brooks
//!
class RealTimeStopwatch : public Stopwatch
{

public: 

    RealTimeStopwatch( bool startOnInitialisation=true );

private: 

    void getTimeNow( struct timeval &now ) const;
};

}

#endif
