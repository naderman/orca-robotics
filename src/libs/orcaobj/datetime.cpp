/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <hydroutil/stringutils.h>
#include <orcaice/timeutils.h>
#include <orcaice/proputils.h>

#include "datetime.h"

using namespace std;

namespace orcaobj
{

// adapted from IceUtil::Time::toDuration()
// the only difference is in NOT printing 'd' after the number of days
// this is to make it easier to parse back
// also dealing with negative time durations
std::string toStringDuration( const orca::Time& obj )
{
    // first decide the sign
    bool isNegative;
    if ( obj.seconds<0 )  {
        assert( obj.useconds<=0 && "seconds and useconds must have the same sign" );
        isNegative = true;
    }
    else {
        assert( obj.useconds>=0 && "seconds and useconds must have the same sign" );
        isNegative = false;
    }

    int secs  = abs(obj.seconds) % 60;
    int mins  = abs(obj.seconds) / 60 % 60;
    int hours = abs(obj.seconds) / 60 / 60 % 24;
    int days  = abs(obj.seconds) / 60 / 60 / 24;

    using namespace std;

    ostringstream os;
    if ( isNegative )
        os << "-";

    if(days != 0)
    {
        os << days << ":";
    }
    os << setfill('0') << setw(2) << hours << ":" << setw(2) << mins << ":" << setw(2) << secs;
    if(obj.useconds != 0)
    {
        os << "." << setw(6) << abs(obj.useconds);
    }

    return os.str();
}

int toTimeDuration( const std::string& s, orca::Time& obj )
{
    if ( s.empty() ) return -1;

    // check for minus sign
    bool isNegative = false;
    if ( s[0]=='-' ) {
//         cout<<"detected negative time"<<endl;
        isNegative = true;
    }

    vector<string> sseq;
    if ( isNegative ) {
        sseq = hydroutil::toStringSeq( s.substr( 1 ) );
    }
    else {
        sseq = hydroutil::toStringSeq( s );
    }
    if ( sseq.empty() ) return -1;
//     cout<<"forward :"<<endl; for ( unsigned int i=0; i<sseq.size(); ++i ) cout<<sseq[i]<<endl;

    // reverse the order so the seconds come first, and the optional elements come last
    std::reverse( sseq.begin(), sseq.end() );
//     cout<<"reversed :"<<endl; for ( unsigned int i=0; i<sseq.size(); ++i ) cout<<sseq[i]<<endl;

    // convert to a single string but now with spaces as delimeters
    std::string s2 = hydroutil::toString( sseq, ' ' );
    std::istringstream ss( s2 );

    IceUtil::Time t;

    double secs;
    ss >> secs;
//     cout<<setprecision(12)<<secs<<" sec"<<endl;
    int sec = (int)floor( secs );
    int usec = (int)round( (secs-sec)*1.0e6 );
//     cout<<sec<<" sec"<<endl;
//     cout<<usec<<" usec"<<endl;
    t = IceUtil::Time::seconds( sec ) + IceUtil::Time::microSeconds( usec );

    int mins;
    ss >> mins;
    if ( !ss ) {
        obj = orcaice::toOrcaTime( t );
        if ( isNegative ) {
            obj.seconds *= -1;
            obj.useconds *= -1;
        }
        return 0;
    }
//     cout<<mins<<" mins"<<endl;
    t += IceUtil::Time::seconds( mins*60 );

    int hours;
    ss >> hours;
    if ( !ss ) {
        obj = orcaice::toOrcaTime( t );
        if ( isNegative ) {
            obj.seconds *= -1;
            obj.useconds *= -1;
        }
        return 0;
    }
//     cout<<hours<<" hrs"<<endl;
    t += IceUtil::Time::seconds( hours*3600 );

    int days;
    ss >> days;
    if ( !ss ) {
        obj = orcaice::toOrcaTime( t );
        if ( isNegative ) {
            obj.seconds *= -1;
            obj.useconds *= -1;
        }
        return 0;
    }
//     cout<<days<<" days"<<endl;
    t += IceUtil::Time::seconds( days*24*3600 );

    obj = orcaice::toOrcaTime( t );
    if ( isNegative ) {
        obj.seconds *= -1;
        obj.useconds *= -1;
    }
    return 0;
}

std::string toString( const orca::Time& obj )
{
    std::ostringstream s;
    s <<orcaice::toIceTime( obj ).toDateTime();
    return s.str();
}

std::string toString( const orca::TimeOfDay& obj )
{
    std::ostringstream s;
    s << "TimeOfDay(h:m:s): " << obj.hours << ":" << obj.minutes << ":" << obj.seconds;
    return s.str();
}

std::string toString( const orca::Date& obj )
{
    std::ostringstream s;
    s << "Date(y,m,d): " << obj.year << ", " << obj.month << ", " << obj.day;
    return s.str();
}

int getPropertyAsTimeDuration( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Time &value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toTimeDuration( stringVal, value );
}

orca::Time getPropertyAsTimeDurationWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Time & defaultValue )
{
    orca::Time value;
    if ( getPropertyAsTimeDuration( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

bool isSane( const orca::Time& obj, std::string& reason )
{
    std::stringstream ss;
    bool sane=true;
    if ( obj.seconds < 0 ) {
        ss << "Negative seconds: " << obj.seconds<<"s"<<endl;
        sane = false;
    }
    if ( obj.useconds < 0 ) {
        ss << "Negative micro seconds: " << obj.useconds<<"us"<<endl;
        sane = false;
    }
    reason = ss.str();
    return sane;
}

} // namespace
