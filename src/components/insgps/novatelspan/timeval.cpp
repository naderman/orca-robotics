/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "timeval.h"

namespace insgps{

    struct timeval* fix_timeval(struct timeval* tv)
{
        //first fix up excessively negative usecs
    while(tv->tv_usec<-999999){
        tv->tv_sec--;
        tv->tv_usec+=1000000;
    }

        //fix up extra large usecs
    while(tv->tv_usec>999999){
        tv->tv_sec++;
        tv->tv_usec-=1000000;
    }

        //correct signs
    if(tv->tv_sec>0 && tv->tv_usec<0){
        tv->tv_sec--;
        tv->tv_usec+=1000000;
    }

        //correct signs
    if(tv->tv_sec<0 && tv->tv_usec>0){
        tv->tv_sec++;
        tv->tv_usec-=1000000;
    }

    return tv;
}
    
struct timeval operator-(const struct timeval& lhs, const struct timeval& rhs)
    {
        struct timeval diff;
        diff.tv_sec =lhs.tv_sec-rhs.tv_sec;
	diff.tv_usec=lhs.tv_usec-rhs.tv_usec;

        fix_timeval(&diff);
        return diff;
    }

    struct timeval operator+(const struct timeval& lhs, const struct timeval& rhs)
    {
	struct timeval sum;
	sum.tv_sec =lhs.tv_sec+rhs.tv_sec;
	sum.tv_usec=lhs.tv_usec+rhs.tv_usec;

        fix_timeval(&sum);
        return sum;
    }

} // namespace
