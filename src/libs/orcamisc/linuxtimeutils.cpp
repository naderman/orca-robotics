/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
// this entire class does not work in Windows
#ifndef WIN32

#include "linuxtimeutils.h"

namespace orcamisc {

timeval 
now() 
{ 
    timeval now; 
    gettimeofday( &now, NULL ); 
    return now; 
}

timeval 
elapsedTimeVal( timeval & startTime )
{ 
    timeval toc;
    gettimeofday( &toc, NULL );
    timeval etime;
    etime.tv_sec = toc.tv_sec-startTime.tv_sec;

    if ( !etime.tv_sec ) { // within the same second
        etime.tv_usec = toc.tv_usec - startTime.tv_usec;
    }else{ // stradling seconds
        etime.tv_usec = toc.tv_usec + 1000000 - startTime.tv_usec;
    }
    return etime;
}

double 
elapsedTimeSec( timeval & startTime )
{ 
    timeval etime = elapsedTimeVal ( startTime );
    return (etime.tv_sec + etime.tv_usec/1000000.0);
}

}

#endif // WIN32
