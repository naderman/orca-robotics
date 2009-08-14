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

#include "cpustopwatch.h"
#include <iostream>
#include <assert.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>

namespace hydroutil {

CpuStopwatch::CpuStopwatch( bool startOnInitialisation )
{
    reset();
    if ( startOnInitialisation )
        start();
}

void
CpuStopwatch::getTimeNow( struct timeval &now ) const
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

#endif
