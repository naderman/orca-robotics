/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
// this entire class does not work in Windows
#ifndef WIN32

#include <iostream>
#include <assert.h>
#include <sys/resource.h>

#include "cpustopwatch.h"

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

namespace orcamisc {

CpuStopwatch::CpuStopwatch( bool startOnInitialisation )
{
    reset();
    if ( startOnInitialisation )
        start();
}

CpuStopwatch::~CpuStopwatch()
{

}

void 
CpuStopwatch::start()
{
    assert( !isRunning_ );
    reset();
    startWithoutReset();
}

void 
CpuStopwatch::startWithoutReset()
{
    assert( !isRunning_ );
    isRunning_ = true;
    getTimeNow( startTime_ );
}

void
CpuStopwatch::addTimeThisRun()
{
    assert( isRunning_ );
    timeval now;
    getTimeNow( now );
    timeval diff;
    // diff = now.startTime_
    TIMESUB( &now, &startTime_, &diff );
    // elapsed += diff
    TIMEADD( &elapsed_, &diff, &elapsed_ );
}

void 
CpuStopwatch::stop()
{
    assert( isRunning_ );
    addTimeThisRun();
    isRunning_ = false;
}

void 
CpuStopwatch::reset()
{
    elapsed_.tv_sec  = 0;
    elapsed_.tv_usec = 0;
    isRunning_ = false;
}

double 
CpuStopwatch::elapsedSeconds()
{
    if ( isRunning_ )
        addTimeThisRun();

    double elapsedSeconds = elapsed_.tv_sec;
    elapsedSeconds += elapsed_.tv_usec*1e-6;
    return elapsedSeconds;
}

void
CpuStopwatch::getTimeNow( struct timeval &now )
{
    rusage usage;
    int ret = getrusage( RUSAGE_SELF, &usage );
    if ( ret != 0 )
    {
        perror( "getrusage failed!" );
        exit(1);
    }
    // Read user time used
    now.tv_sec  = usage.ru_utime.tv_sec;
    now.tv_usec = usage.ru_utime.tv_usec;
}

}

#endif // WIN32
