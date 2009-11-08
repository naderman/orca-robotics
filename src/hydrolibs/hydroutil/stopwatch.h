/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROUTIL_STOPWATCH_H
#define HYDROUTIL_STOPWATCH_H

#include <sys/time.h>

namespace hydroutil {

//!
//! @brief base class for timing something.
//!
//! The 'start' and 'stop' buttons do the obvious thing.
//! 'startWithoutReset' starts the clock ticking _without_ resetting first.
//! 'reset' resets and stops the clock.
//!
//! @sa CpuStopwatch
//! @sa RealTimeStopwatch
//!
//! @author Alex Brooks
//!
class Stopwatch
{

public: 

    virtual ~Stopwatch() {};

    void start();
    void stop();
    void startWithoutReset();
    
    double elapsedSeconds() const;
    double elapsedMs() const { return elapsedSeconds()*1000.0; }
    void reset();

protected:

    // Overload this according to the type of time we're measuring.
    virtual void getTimeNow( struct timeval &now ) const=0;

private: 

    bool   isRunning_;

    timeval elapsed_;
    timeval startTime_;

    // Call this only when the clock's ticking.
    void addTimeThisRun( timeval &accumulator ) const;
};

}

#endif
