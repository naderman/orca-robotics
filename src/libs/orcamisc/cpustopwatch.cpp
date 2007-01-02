#include "cpustopwatch.h"
#include <iostream>
#include <assert.h>
#include <sys/resource.h>

namespace orcamisc {

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
