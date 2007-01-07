#include "basics2.h"
#include "timelib.h"
#include <time.h>

namespace laser2d {

//#define xuint32 unsigned int 
//32 bits unsigned

static xuint32 ttOffsetZZSecs=0,ttOffsetZZNanoSecs=0 ;
static int UseTimeOffset=1 ;

void IniGetTimeUTE1(void)
{
struct timespec tp ;
clock_gettime(CLOCK_MONOTONIC,&tp) ;
if (UseTimeOffset) { ttOffsetZZSecs = (xuint32)tp.tv_sec ;}
else{ ttOffsetZZSecs = 0 ;}
ttOffsetZZNanoSecs = 0 ;
}

xuint32 GetTimeUTE1(void)
{ struct timespec tp ;
xuint32 tt ;
clock_gettime(CLOCK_MONOTONIC,&tp) ;
tt = ( ((xuint32)tp.tv_sec) - ttOffsetZZSecs ) * 10000 + (((xuint32)tp.tv_nsec)-ttOffsetZZNanoSecs)/100000;
return(tt) ;
}

xuint32 GetTimeUTE1ms(void)
{ struct timespec tp ;
xuint32 tt ;
clock_gettime(CLOCK_MONOTONIC,&tp) ;
tt = ( ((xuint32)tp.tv_sec) - ttOffsetZZSecs ) * 1000 + (((xuint32)tp.tv_nsec)-ttOffsetZZNanoSecs)/1000000;
return(tt) ;
}

} // namespace
