/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <cmath>    // floor()

#include <IceUtil/Time.h>

// for toOrcaTimeOfDay() and toOrcaDate()
#ifdef _WIN32
#   include <sys/timeb.h>
#   include <time.h>
#else
#   include <sys/time.h>
#endif

#include "timeutils.h"

using namespace std;

namespace orcaice
{

IceUtil::Time 
toIceTime( const orca::Time & orcatime )
{
    return IceUtil::Time::seconds(orcatime.seconds) + IceUtil::Time::microSeconds(orcatime.useconds);
}

orca::Time 
toOrcaTime( const IceUtil::Time & icetime )
{
    orca::Time orcatime;
    //! @todo are these casts ok?
    orcatime.seconds = (int)icetime.toSeconds();
    orcatime.useconds = (int)icetime.toMicroSeconds() - (int)icetime.toSeconds()*1000000;
    return orcatime;
}

orca::Time 
toOrcaTime( double sec )
{
    // first decide the sign
    int sign;
    if ( sec<0 ) 
        sign = -1;
    else
        sign = 1;

    orca::Time orcatime;

    orcatime.seconds = (int)floor( fabs(sec) );
    orcatime.useconds = (int)floor( (fabs(sec) - (double)orcatime.seconds) * 1000000 );

    orcatime.seconds *= sign;
    orcatime.useconds *= sign;
    return orcatime;
}

orca::Time 
getNow()
{
    return toOrcaTime( IceUtil::Time::now() );
}

void 
setToNow( orca::Time &obj )
{
    obj = toOrcaTime( IceUtil::Time::now() );
}

orca::TimeOfDay
toOrcaTimeOfDay( const IceUtil::Time & icetime )
{
    // adapterd from IceUtil::Time::toDateTime() in <Ice>/src/IceUtil/Time.cpp
    time_t time = static_cast<long>(icetime.toMicroSeconds() / 1000000);

    // use UTC time
    struct tm* t;
#ifdef WIN32
//     t = localtime(&time);
    t = gmtime(&time);
#else
    struct tm tr;
//     localtime_r(&time, &tr);
    gmtime_r(&time, &tr);
    t = &tr;
#endif

    orca::TimeOfDay timeOfDay;
    timeOfDay.hours = t->tm_hour;
    timeOfDay.minutes = t->tm_min;
    timeOfDay.seconds = t->tm_sec;

    // add fractional seconds remaining in the Ice time structure
    timeOfDay.seconds += (double)(icetime.toMicroSeconds()-icetime.toSeconds()*1000000)/1000000.0;
    
    return timeOfDay;
}

orca::Date
toOrcaDate( const IceUtil::Time & icetime )
{
    // adapterd from IceUtil::Time::toDateTime() in <Ice>/src/IceUtil/Time.cpp
    time_t time = static_cast<long>(icetime.toMicroSeconds() / 1000000);

    // use UTC time
    struct tm* t;
#ifdef WIN32
//     t = localtime(&time);
    t = gmtime(&time);
#else
    struct tm tr;
//     localtime_r(&time, &tr);
    gmtime_r(&time, &tr);
    t = &tr;
#endif

    orca::Date date;
    // tm_year: The number of years since 1900
    date.year = 1900 + t->tm_year;
    // tm_mon: The number of months since January, in the range 0 to 11.
    date.month = 1 + t->tm_mon;
    date.day = t->tm_mday;

    return date;
}

orca::Time 
timeDiff( const orca::Time &t1, const orca::Time &t2 )
{
    return toOrcaTime( timeDiffAsDouble( t1, t2 ) );
}

double 
timeDiffAsDouble( const orca::Time &t1, const orca::Time &t2 )
{
    return ( double(t1.seconds - t2.seconds) + 1.0e-6 * double(t1.useconds - t2.useconds) );
}

double 
timeAsDouble( const orca::Time &t )
{
    return ((double)(t.seconds) + 1.0e-6 * (double)(t.useconds));
}

void add( orca::Time &t, double seconds )
{
    int numSeconds      = (int)seconds;
    int numMicroSeconds = (int)( (seconds-numSeconds)*1e6 );

    t = toOrcaTime( toIceTime(t) +
                    IceUtil::Time::seconds(numSeconds) +
                    IceUtil::Time::microSeconds(numMicroSeconds) );
}
void subtract( orca::Time &t, double seconds )
{
    int numSeconds      = (int)seconds;
    int numMicroSeconds = (int)( (seconds-numSeconds)*1e6 );

    t = toOrcaTime( toIceTime(t) -
                    IceUtil::Time::seconds(numSeconds) -
                    IceUtil::Time::microSeconds(numMicroSeconds) );    
}


} // namespace
