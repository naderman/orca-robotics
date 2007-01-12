/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "sickdefines.h"
#include "time.h"
#include <time.h>

//#define xuint32 unsigned int 
//32 bits unsigned

static xuint32 ttOffsetZZSecs=0,ttOffsetZZNanoSecs=0 ;
// Use a value of 0 for system time or 1 to start the clock at 0
static int UseTimeOffset = 0;

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

