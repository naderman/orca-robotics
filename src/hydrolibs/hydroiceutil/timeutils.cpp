/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <iomanip>

#include "timeutils.h"

namespace hydroiceutil
{

void now( int& seconds, int& useconds )
{
    IceUtil::Time t = IceUtil::Time::now();
    seconds = (int)t.toSeconds();
    useconds = (int)t.toMicroSeconds() - seconds*1000000;
}

void timeFromIceUtil( const IceUtil::Time& t, int& seconds, int& useconds )
{
    seconds = (int)t.toSeconds();
    useconds = (int)t.toMicroSeconds() - seconds*1000000;
}

// adapted from IceUtil::Time::toDuration()
// the only difference is in NOT printing 'd' after the number of days
// this is to make it easier to parse back
// also dealing with negative time durations
std::string toDuration( const IceUtil::Time& t, const std::string& separator )
{
    int useconds = t.toMicroSeconds();

    int usecs = useconds % 1000000;
    int secs = useconds / 1000000 % 60;
    int mins = useconds / 1000000 / 60 % 60;
    int hours = useconds / 1000000 / 60 / 60 % 24;
    int days = useconds / 1000000 / 60 / 60 / 24;

    using namespace std;

    ostringstream os;

    os << days << separator
       << hours << separator 
       << mins << separator 
       << secs << "."
       << (usecs / 1000);

    return os.str();
}

void timeFromDouble( double time, int& seconds, int& useconds )
{
    // first decide on the sign
    int sign;
    if ( time<0 )
        sign = -1;
    else
        sign = 1;

    seconds = (int)floor( fabs(time) );
    useconds = (int)floor( (fabs(time) - (double)seconds) * 1000000 );

    seconds *= sign;
    useconds *= sign;
}

IceUtil::Time timeFromDouble( double time )
{
    int seconds, useconds;
    timeFromDouble( time, seconds, useconds );
    return IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds);
}

double timeAsDouble( int seconds, int useconds )
{
    return ((double)(seconds) + 1.0e-6 * (double)(useconds));
}

} // namespace
