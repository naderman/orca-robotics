/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROUTIL_CPUSTOPWATCH_H
#define HYDROUTIL_CPUSTOPWATCH_H

#include <hydroutil/stopwatch.h>

namespace hydroutil {

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
