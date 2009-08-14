/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#if !defined( WIN32 )

#include "stopwatch.h"
#include <iostream>
#include <assert.h>

////////// Helper Macros for manipulating timevalues ///////////

// does: result = a - b;
//
#define TIMESUB(a, b, result)                                         \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)

// does: result = a + b;
//
#define TIMEADD(a, b, result)                                         \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                          \
    if ((result)->tv_usec > 1000000) {                                        \
      ++(result)->tv_sec;                                                     \
      (result)->tv_usec -= 1000000;                                           \
    }                                                                         \
  } while (0)

////////////////////////////////////////////////////////////////////////////////

using namespace std;
namespace hydroutil {

void 
Stopwatch::start()
{
    reset();
    startWithoutReset();
}

void 
Stopwatch::startWithoutReset()
{
    assert( !isRunning_ );
    isRunning_ = true;
    getTimeNow( startTime_ );
}

void
Stopwatch::addTimeThisRun( timeval &accumulator ) const
{
    assert( isRunning_ );
    timeval now;
    getTimeNow( now );
    timeval diff;
    // diff = now.startTime_
    TIMESUB( &now, &startTime_, &diff );
    // elapsed += diff
    TIMEADD( &accumulator, &diff, &accumulator );
}

void 
Stopwatch::stop()
{
    assert( isRunning_ );
    addTimeThisRun( elapsed_ );
    isRunning_ = false;
}

void 
Stopwatch::reset()
{
    elapsed_.tv_sec  = 0;
    elapsed_.tv_usec = 0;
    isRunning_ = false;
}

double 
Stopwatch::elapsedSeconds() const
{
    timeval el = elapsed_;
    if ( isRunning_ )
        addTimeThisRun( el );

    double elapsedSeconds = el.tv_sec;
    elapsedSeconds += el.tv_usec*1e-6;
    return elapsedSeconds;
}

}
#endif
