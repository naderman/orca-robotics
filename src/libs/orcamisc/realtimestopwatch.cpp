/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __WIN32__

#include "realtimestopwatch.h"
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <iostream>
using namespace std;

namespace orcamisc {

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
