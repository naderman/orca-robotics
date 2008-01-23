/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <sstream>
#include <iomanip>
#include <hydroutil/stringutils.h>
#include <hydroutil/mathdefs.h>
#include <orcaice/timeutils.h>

#include "stringutils.h"

using namespace std;

namespace orcaice
{

std::string 
toString( const orca::FQComponentName& name )
{
    return name.platform + "/" + name.component;
}

std::string 
toString( const orca::FQInterfaceName& name )
{
    return name.iface + "@" + name.platform + "/" + name.component;
}

std::string 
toString( const orca::FQTopicName& name )
{
    return name.iface + "/" + name.topic + "@" + name.platform + "/" + name.component;
}

std::string toString( const orca::FQExecutableName& name )
{
    return name.executable + "@" + name.host ;
}

orca::FQComponentName 
toComponentName( const std::string& s )
{
    orca::FQComponentName fqCName;

    int posSlash = s.find( '/', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posSlash<0 ) { 
        // delimeter not found: return null structure
    }
    else {
        fqCName.platform = s.substr( 0, posSlash );
        fqCName.component = s.substr( posSlash+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqCName)<<endl;
    return fqCName;
}

orca::FQInterfaceName 
toInterfaceName( const std::string& s )
{
    orca::FQInterfaceName fqIName;

    int posAt = s.find( '@', 0 );
    //cout<<"@ found :"<<posAt<<endl;
    int posSlash = s.find( '/', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posAt<0 || posSlash<0 ) { 
        // delimeters not found: return null structure
    }
    else {
        fqIName.iface = s.substr( 0, posAt );
        fqIName.platform = s.substr( posAt+1, posSlash-posAt-1 );
        fqIName.component = s.substr( posSlash+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqIName)<<endl;
    return fqIName;
}

std::string 
toHomeIdentity( const orca::FQComponentName & fqCName )
{
    return "orca." + fqCName.platform + "." + fqCName.component + "/Home";
}

orca::FQTopicName 
toStatusTopic( const orca::FQComponentName& fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform  = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface     = "status";
    fqTName.topic     = "*";

    return fqTName;
}

orca::FQTopicName 
toTracerTopic( const orca::FQComponentName& fqCName )
{
    orca::FQTopicName fqTName;
        
    fqTName.platform  = fqCName.platform;
    fqTName.component = fqCName.component;
    fqTName.iface     = "tracer";
    fqTName.topic     = "*";

    return fqTName;
}


// adapted from IceUtil::Time::toDuration()
// the only difference is in NOT printing 'd' after the number of days
// this is to make it easier to parse back
// also dealing with negative time durations
std::string 
toStringDuration( const orca::Time& obj )
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

int 
toTimeDuration( const std::string& s, orca::Time& obj )
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
        obj = toOrcaTime( t );
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
        obj = toOrcaTime( t );
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
        obj = toOrcaTime( t );
        if ( isNegative ) {
            obj.seconds *= -1;
            obj.useconds *= -1;
        }
        return 0;
    }
//     cout<<days<<" days"<<endl;
    t += IceUtil::Time::seconds( days*24*3600 );

    obj = toOrcaTime( t );
    if ( isNegative ) {
        obj.seconds *= -1;
        obj.useconds *= -1;
    }
    return 0;
}

std::string 
toString( const orca::Time& obj )
{
    std::ostringstream s;
    s <<toIceTime( obj ).toDateTime();
    return s.str();
}

std::string 
toString( const orca::TimeOfDay& obj )
{
    std::ostringstream s;
    s << "TimeOfDay(h:m:s): " << obj.hours << ":" << obj.minutes << ":" << obj.seconds;
    return s.str();
}

std::string 
toString( const orca::Date& obj )
{
    std::ostringstream s;
    s << "Date(y,m,d): " << obj.year << ", " << obj.month << ", " << obj.day;
    return s.str();
}




int toIntVector( const std::string& s, std::vector<int>& obj )
{
    std::istringstream ss( s );
    
    //check that there's something in the input stream
    if ( !ss )
    {
        return -1;
    }
    
    int tmp;
    while ( true )
    {
        ss >> tmp;
        
        // if we're not at the end of the line but something went wrong, return error
        if ( !ss && !ss.eof() )
        {
            return -1;
        }
        
        obj.push_back( tmp );
    
        // check if we're at the end of the line
        if ( ss.eof() )
        {
            break;
        }
    }
    
    // success
    return 0;
}

int toDoubleVector( const std::string& s, std::vector<double>& obj )
{
    std::istringstream ss( s );
    
    //check that there's something in the input stream
    if ( !ss )
    {
        return -1;
    }
    
    double tmp;
    while ( true )
    {
        ss >> tmp;
        
        // if we're not at the end of the line but something went wrong, return error
        if ( !ss && !ss.eof() )
        {
            return -1;
        }
        
        obj.push_back( tmp );
    
        // check if we're at the end of the line
        if ( ss.eof() )
        {
            break;
        }
    }
    
    // success
    return 0;
}

} // namespace
