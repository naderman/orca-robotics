#include "time.h"
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <sstream>
#include <cmath>
#include <sys/time.h>
#include <assert.h>

using namespace std;

namespace hydrotime {

Time 
getNow()
{
    timeval time;
    int ret = gettimeofday( &time, NULL );
    if ( ret != 0 )
    {
        perror( "gettimeofday" );
        assert( false && "gettimeofday failed" );
    }
    return Time( time.tv_sec, time.tv_usec );
}

Time 
toTime( double sec )
{
    // first decide the sign
    int sign;
    if ( sec<0 ) 
        sign = -1;
    else
        sign = 1;

    Time time;

    time.seconds() = (int)floor( fabs(sec) );
    time.useconds() = (int)floor( (fabs(sec) - (double)time.seconds()) * 1000000 );

    time.seconds() *= sign;
    time.useconds() *= sign;
    return time;
}

std::string toString( const Time &t )
{
    time_t time = t.seconds();

    struct tm* tm_time;
#ifdef _WIN32
    tm_time = localtime(&time);
#else
    struct tm tr;
    localtime_r(&time, &tr);
    tm_time = &tr;
#endif

    char buf[32];
    strftime(buf, sizeof(buf), "%x %H:%M:%S", tm_time);

    char mSecAnduSec[32];
    sprintf(mSecAnduSec,"%03d.%03d",t.useconds()/1000,t.useconds()%1000);
    stringstream os;
    os << buf << "." << mSecAnduSec;
    return os.str();
}

std::string toStringAsSecUSec( const Time &t )
{
    stringstream ss;
    ss << t.seconds() << ":" << t.useconds();
    return ss.str();
}

void add( Time &t, double secondsAsDouble )
{
    if ( secondsAsDouble < 0 )
    {
        subtract(t,-secondsAsDouble);
        return;
    }

    int numSeconds      = (int)secondsAsDouble;
    int numMicroSeconds = (int)( (secondsAsDouble-numSeconds)*1e6 );
    assert( numMicroSeconds <= 1e6 );

    t.useconds() += numMicroSeconds;
    if ( t.useconds() >= 1e6 )
    {
        t.useconds() -= 1e6;
        t.seconds() += numSeconds+1;
    }
    else
    {
        t.seconds() += numSeconds;
    }
}

void subtract( Time &t, double secondsAsDouble )
{
    if ( secondsAsDouble < 0 )
    {
        add(t,-secondsAsDouble);
        return;
    }

    int numSeconds      = (int)secondsAsDouble;
    int numMicroSeconds = (int)( (secondsAsDouble-numSeconds)*1e6 );

    assert( numMicroSeconds <= 1e6 );

    t.useconds() -= numMicroSeconds;
    if ( t.useconds() < 0 )
    {
        t.useconds() += 1e6;
        t.seconds() -= numSeconds+1;
    }
    else
    {
        t.seconds() -= numSeconds;
    }
}

void add( Time &t1, const Time &t2 )
{
    t1.useconds() += t2.useconds();
    if ( t1.useconds() >= 1e6 )
    {
        t1.useconds() -= 1e6;
        t1.seconds() += t2.seconds()+1;
    }
    else
    {
        t1.seconds() += t2.seconds();
    }
}

void subtract( Time &t1, const Time &t2 )
{
    t1.useconds() -= t2.useconds();
    if ( t1.useconds() < 0 )
    {
        t1.useconds() += 1e6;
        t1.seconds() -= t2.seconds()+1;
    }
    else
    {
        t1.seconds() -= t2.seconds();
    }
}

}

