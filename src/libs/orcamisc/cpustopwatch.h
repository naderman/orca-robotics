/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_MISC_CPUSTOPWATCH_H
#define ORCA2_MISC_CPUSTOPWATCH_H

#include <sys/time.h>

namespace orcamisc {

//!
//! Does not work in Windows!
//!
//! This fucker times _user-space_cpu_time_, not wallclock time.
//! This should give some idea of how hard your process is working, relatively
//! independent of system load or how much time is spent waiting for I/O.
//!
//! The 'start' and 'stop' buttons do the obvious thing.
//! 'startWithoutReset' starts the clock ticking _without_ resetting first.
//! 'reset' resets and stops the clock.
//!
//! @author Alex Brooks
//!
class CpuStopwatch
{

public: 

    CpuStopwatch( bool startOnInitialisation=false);
    ~CpuStopwatch();

    void start();
    void stop();
    void startWithoutReset();
    
    double elapsedSeconds();
    void reset();

private: 

    bool   isRunning_;

    timeval elapsed_;
    timeval startTime_;

    // Call this only when the clock's ticking.
    void addTimeThisRun();

    void getTimeNow( struct timeval &now );
};

}

#endif
