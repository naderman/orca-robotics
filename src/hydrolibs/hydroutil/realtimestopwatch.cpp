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

#include "realtimestopwatch.h"
#include <cstdlib>
#include <sys/time.h>
#include <time.h>
#include <cstring>
#include <errno.h>
#include <iostream>
using namespace std;

namespace hydroutil {

RealTimeStopwatch::RealTimeStopwatch( bool startOnInitialisation )
{
    reset();
    if ( startOnInitialisation )
        start();
}

void
RealTimeStopwatch::getTimeNow( struct timeval &now ) const
{
    int ret = gettimeofday( &now, 0 );
    if ( ret != 0 )
    {
        cout << "ERROR(realtimestopwatch.cpp): problem getting timeofday: " 
             << strerror(errno) << endl;
        exit(1);
    }
}

}
#endif
