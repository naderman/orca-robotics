/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <hydroutil/exceptions.h>
#include <hydroutil/stringutils.h>

// trying to solve a problem in win, round() is not found
#ifdef WIN32
#   include <math.h>
inline double round( double d )
{
	return floor( d + 0.5 );
}
#else
#   include <cmath>
#endif

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#include <orcaobj/timeutils.h>
#include <orcaobj/miscutils.h>
#include <hydroutil/mathdefs.h>

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


//**************************
// toString()
//*************************

std::string 
toString( const orca::CartesianPoint2d& obj )
{
    std::ostringstream s;
    s << obj.x << " " << obj.y << " x[m], y[m]";
    return s.str();
}

int 
toCartesianPoint2d( const std::string& s, orca::CartesianPoint2d& obj )
{
    std::istringstream ss( s );
    ss >> obj.x;
    if ( !ss ) return -1;
    ss >> obj.y;
    if ( !ss ) return -1;
    // success
    return 0;
}

std::string 
toString( const orca::CartesianPoint& obj )
{
    std::ostringstream s;
    s << obj.x << " " << obj.y << " " << obj.z << " x[m], y[m], z[m]";
    return s.str();
}

int 
toCartesianPoint( const std::string& s, orca::CartesianPoint& obj )
{
    std::istringstream ss( s );
    ss >> obj.x;
    if ( !ss ) return -1;
    ss >> obj.y;
    if ( !ss ) return -1;
    ss >> obj.z;
    if ( !ss ) return -1;
    // success
    return 0;
}

std::string 
toString( const orca::Frame2d& obj )
{
    std::ostringstream s;
//     s << "[" << obj.p.x << "m," << obj.p.y << "m," << obj.o*180.0/M_PI << "deg]";
    s << obj.p.x << " " << obj.p.y << " " << obj.o*180.0/M_PI << " x[m], y[m], yaw[deg]";
    return s.str();
}

int 
toFrame2d( const std::string& s, orca::Frame2d& obj )
{
    std::istringstream ss( s );
    ss >> obj.p.x;
    if ( !ss ) return -1;
    ss >> obj.p.y;
    if ( !ss ) return -1;
    ss >> obj.o;
    if ( !ss ) return -1;
    obj.o *= DEG2RAD_RATIO;
    // success
    return 0;
}

std::string 
toString( const orca::Frame3d& obj )
{
    std::ostringstream s;
//     s << "[" <<obj.p.x<<"m," <<obj.p.y<<"m," <<obj.p.z<<"m, " 
//       << obj.o.r*180.0/M_PI<<"," <<obj.o.p*180.0/M_PI<<"," <<obj.o.y*180.0/M_PI << "deg]";
    s << obj.p.x << " " << obj.p.y << " " << obj.p.z << "   " 
      << obj.o.r*180.0/M_PI << " "<< obj.o.p*180.0/M_PI << " "<< obj.o.y*180.0/M_PI 
      << " x[m], y[m], z[m],   roll[deg], pitch[deg], yaw[deg]";
    return s.str();
}

int 
toFrame3d( const std::string& s, orca::Frame3d& obj )
{
    std::istringstream ss( s );
    ss >> obj.p.x;
    if ( !ss ) return -1;
    ss >> obj.p.y;
    if ( !ss ) return -1;
    ss >> obj.p.z;
    if ( !ss ) return -1;
    ss >> obj.o.r;
    if ( !ss ) return -1;
    obj.o.r *= DEG2RAD_RATIO;
    ss >> obj.o.p;
    if ( !ss ) return -1;
    obj.o.p *= DEG2RAD_RATIO;
    ss >> obj.o.y;
    if ( !ss ) return -1;
    obj.o.y *= DEG2RAD_RATIO;
    // success
    return 0;
}

std::string 
toString( const orca::Size2d& obj )
{
    std::ostringstream s;
    s << obj.l << " " << obj.w << " length[m], width[m]";
    return s.str();
}

int 
toSize2d( const std::string& s, orca::Size2d& obj )
{
    std::istringstream ss( s );
    ss >> obj.l;
    if ( !ss ) return -1;
    ss >> obj.w;
    if ( !ss ) return -1;
    // success
    return 0;
}

std::string 
toString( const orca::Size3d& obj )
{
    std::ostringstream ss;
    ss << obj.l << " " << obj.w << " " << obj.h << " length[m], width[m], height[m]";
    return ss.str();
}

int 
toSize3d( const std::string& s, orca::Size3d& obj )
{
    std::istringstream ss( s );
    ss >> obj.l;
    if ( !ss ) return -1;
    ss >> obj.w;
    if ( !ss ) return -1;
    ss >> obj.h;
    if ( !ss ) return -1;
    // success
    return 0;
}

int 
toIntVector( const std::string& s, std::vector<int>& obj )
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

int 
toDoubleVector( const std::string& s, std::vector<double>& obj )
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

std::string 
toString( const orca::BinarySwitchData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " BinarySwitch ["<<obj.devices.size()<<" devices] (name,state) :";

    for ( unsigned int i=0; i < obj.devices.size(); i++ )
    {
        s << endl << "    " << i << " [" 
          <<obj.devices[i].name<<","
          <<(int)obj.devices[i].state<<"]";
    }
    return s.str();
}

std::string 
toString( const orca::CameraData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " CameraData: ["<<obj.image.size()<<" bytes]\n" 
        << "Image height              " << obj.imageHeight << "pix\n"
        << "Image width               " << obj.imageWidth << "pix\n"
        << "Format                    " << obj.format << "\n"
        << "Compression               " << obj.compression << "\n";
    return s.str();
}

std::string 
toString( const orca::CameraDescription& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " CameraDescription:\n" 
        << "Image height              " << obj.imageHeight << "pix\n"
        << "Image width               " << obj.imageWidth << "pix\n"
        << "Frame rate                " << obj.frameRate << "fps\n"
        << "Format                    " << obj.format << "\n"
        << "Compression               " << obj.compression << "\n"
        << "offset.point.x            " << obj.offset.p.x << "m\n"
        << "offset.point.y            " << obj.offset.p.y << "m\n"
        << "offset.point.z            " << obj.offset.p.z << "m\n"
        << "offset.orientation.roll   " << RAD2DEG(obj.offset.o.r) << "deg\n"
        << "offset.orientation.pitch  " << RAD2DEG(obj.offset.o.p) << "deg\n"
        << "offset.orientation.yaw    " << RAD2DEG(obj.offset.o.y) << "deg\n";
    return s.str();
}

std::string 
toString( const orca::CpuInfo& obj )
{
    std::ostringstream s;
    //s << toString(obj.timeStamp)
    s  << " CpuInfo: info = " << obj.info <<endl;

    return s.str();
}

std::string 
toString( const orca::CpuData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp);
    s  << " CpuData: user = " << obj.userLoad <<"%";
    s << " sys = " << obj.systemLoad <<"%";
    s << " idle = " << obj.idleLoad <<"%";
    s << " temp = " << obj.temperature <<"C"<<endl;

    return s.str();
}

std::string 
toString( const orca::DriveBicycleCommand& obj )
{
    std::ostringstream s;
    s  << " DriveBicycleCommand: timeStamp=" << toString(obj.timeStamp) <<"s";
    s  << " speed=" << obj.speed <<"m";
    s << " steerAngle=" << RAD2DEG(obj.steerAngle) <<"deg";
    return s.str();
}

std::string 
toString( const orca::DriveBicycleData& obj )
{
    std::ostringstream s;
    s  << " DriveBicycleData: timeStamp=" << toString(obj.timeStamp) <<"s";
    s  << " ref speed=" << obj.referenceSpeed <<"m";
    s << " ref steerAngle=" << RAD2DEG(obj.referenceSteerAngle) <<"deg";
    s  << " current speed=" << obj.currentSpeed <<"m";
    s << " current steerAngle=" << RAD2DEG(obj.currentSteerAngle) <<"deg";
    return s.str();
}

std::string 
toString( const orca::EStopData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp);
    s << " EStop fault condition is:- "<< obj.hasFaultCondition; 
    return s.str();
}

std::string 
toString( const orca::GpsDescription& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " GpsDescription: \n"
        << "antennaOffset.point.x            " << obj.antennaOffset.p.x << "m\n"
        << "antennaOffset.point.y            " << obj.antennaOffset.p.y << "m\n"
        << "antennaOffset.point.z            " << obj.antennaOffset.p.z << "m\n"
        << "antennaOffset.orientation.roll   " << RAD2DEG(obj.antennaOffset.o.r) << "deg\n"
        << "antennaOffset.orientation.pitch  " << RAD2DEG(obj.antennaOffset.o.p) << "deg\n"
        << "antennaOffset.orientation.yaw    " << RAD2DEG(obj.antennaOffset.o.y) << "deg\n";
    return s.str();
}

std::string
toString( const orca::GpsPositionType &p )
{
    switch ( p )
    {
    case orca::GpsPositionTypeNotAvailable:
        return "GpsPositionTypeNotAvailable";
    case orca::GpsPositionTypeAutonomous:
        return "GpsPositionTypeAutonomous";
    case orca::GpsPositionTypeDifferential:
        return "GpsPositionTypeDifferential";
    case orca::NovatelNone:
        return "NovatelNone";
    case orca::NovatelFixedPos:
        return "NovatelFixedPos";
    case orca::NovatelFixedHeigth:
        return "NovatelFixedHeigth";
    case orca::NovatelFloatConv:
        return "NovatelFloatConv";
    case orca::NovatelWideLane:
        return "NovatelWideLane";
    case orca::NovatelNarrowLane:
        return "NovatelNarrowLane";
    case orca::NovatelDopplerVelocity:
        return "NovatelDopplerVelocity";
    case orca::NovatelSingle:
        return "NovatelSingle";
    case orca::NovatelPsrDiff:
        return "NovatelPsrDiff";
    case orca::NovatelWAAS:
        return "NovatelWAAS";
    case orca::NovatelPropagated:
        return "NovatelPropagated";
    case orca::NovatelOmnistar:
        return "NovatelOmnistar";
    case orca::NovatelL1Float:
        return "NovatelL1Float";
    case orca::NovatelIonFreeFloat:
        return "NovatelIonFreeFloat";
    case orca::NovatelNarrowFloat:
        return "NovatelNarrowFloat";
    case orca::NovatelL1Int:
        return "NovatelL1Int";
    case orca::NovatelWideInt:
        return "NovatelWideInt";
    case orca::NovatelNarrowInt:
        return "NovatelNarrowInt";
    case orca::NovatelRTKDirectINS:
        return "NovatelRTKDirectINS";
    case orca::NovatelINS:
        return "NovatelINS";
    case orca::NovatelINSPSRSP:
        return "NovatelINSPSRSP";
    case orca::NovatelINSPSRFLOAT:
        return "NovatelINSPSRFLOAT";
    case orca::NovatelINSRTKFLOAT:
        return "NovatelINSRTKFLOAT";
    case orca::NovatelINSRTKFIXED:
        return "NovatelINSRTKFIXED";
    case orca::NovatelOmnistarHP:
        return "NovatelOmnistarHP";
    default:
        return "??Unknown??";
    }
    // assert( false && "Unknown GpsPositionType" );
}

std::string 
toString( const orca::GpsData&  obj )
{
    std::ostringstream s;
    s   << toString(obj.timeStamp) <<"\n"
        << " UTC: " << toString(obj.utcTime) << "\n"
        << " Gps (latitude,longitude,altitude) : ("
        << obj.latitude << ","
        << obj.longitude << ","
        << obj.altitude << ")" << "\n"
        << " Gps (horizontalPositionError,verticalPositionError) : ("
        << std::setprecision(6)
        << obj.horizontalPositionError<< ","
        << obj.verticalPositionError << "\n"
        << " Gps (heading,speed,climbrate) : ("
        << obj.heading << ","
        << obj.speed << ","
        << obj.climbRate << ")" << "\n"
        << " Gps (satellites,l1,l2,positionType,geoidalSeparation) : ("
        << obj.satellites << ","
        << obj.observationCountOnL1 << ","
        << obj.observationCountOnL2 << ","
        << toString(obj.positionType) << ","
        << obj.geoidalSeparation << ")";

    return s.str();
}

std::string 
toString( const orca::ImageDataPtr& obj )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
        << " ImageData:\n" 
        << "Image height              " << obj->height << "pix\n"
        << "Image width               " << obj->width << "pix\n"
        << "Format                    " << obj->format << "\n"
        << "Compression               " << obj->compression << "\n";
    return s.str();
}

std::string 
toString( const orca::ImageDescription& obj )
{
    std::ostringstream s;
    s << "ImageDescription:\n" 
        << "Image height              " << obj.imageHeight << "pix\n"
        << "Image width               " << obj.imageWidth << "pix\n"
        << "Format                    " << obj.format << "\n"
        << "Compression               " << obj.compression << "\n";
    return s.str();
}

std::string 
toString( const orca::Odometry2dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " Odometry2d (x,y,a(deg), vx,vy,w(deg/s)) : ("
        << obj.pose.p.x << ","
        << obj.pose.p.y << ","
        << RAD2DEG(obj.pose.o) << ", "
        << obj.motion.v.x << ","
        << obj.motion.v.y << ","
        << RAD2DEG(obj.motion.w) << ")";

    return s.str();
}

std::string 
toString( const orca::Odometry3dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
    << setprecision(15)
    << "Position (x,y,z): "
    << obj.pose.p.x << ","
    << obj.pose.p.y << ","
    << obj.pose.p.z << "\n"
    << "Attitude r,p,y(deg): "     
    << RAD2DEG(obj.pose.o.r) << ", "
    << RAD2DEG(obj.pose.o.p) << ", "
    << RAD2DEG(obj.pose.o.y) << "\n"
    << "Velocity vx,vy,vz: "
    << obj.motion.v.x << ","
    << obj.motion.v.y << ","
    << obj.motion.v.z << "\n"
    << "Angular Velocity wx,wy,wz(deg/s): "     
    << RAD2DEG(obj.motion.w.x) << ","
    << RAD2DEG(obj.motion.w.y) << ","
    << RAD2DEG(obj.motion.w.z);

    return s.str();
}

std::string 
toString( const orca::VehicleDescription& obj )
{
    std::ostringstream s;
    s << "VehicleDescription: " << endl
      << "  " << toString(obj.control) << endl
      << "  platformToVehicleTransform: " << toString(obj.platformToVehicleTransform) << endl
      << toString(obj.geometry);
    return s.str();
}

std::string 
toString( const orca::VehicleControlDescriptionPtr& obj )
{
    if ( obj == 0 )
        throw hydroutil::Exception( ERROR_INFO, "VehicleControlDescriptionPtr was zero!" );

    std::ostringstream s;
    s << "VehicleControlDescription: "
      << "type=";
    if ( obj->type == orca::VehicleControlVelocityDifferential )
        s << "VehicleControlVelocityDifferential";
    else if ( obj->type == orca::VehicleControlVelocityBicycle )
        s << "VehicleControlVelocityBicycle";
    else if ( obj->type == orca::VehicleControlVelocitySynchro )
        s << "VehicleControlVelocitySynchro";
    else if ( obj->type == orca::VehicleControlVelocityAckerman )
        s << "VehicleControlVelocityAckerman";
    else if ( obj->type == orca::VehicleControlOther )
        s << "VehicleControlOther";
    else
        s << "Unknown!!";

    if ( obj->ice_isA( "::orca::VehicleControlVelocityDifferentialDescription" ) )
    {
        orca::VehicleControlVelocityDifferentialDescriptionPtr v = orca::VehicleControlVelocityDifferentialDescriptionPtr::dynamicCast( obj );
        if ( v )
        {
            s << endl << "VehicleControlVelocityDifferentialDescription: " << endl
            << "     maxForwardSpeed:           " << v->maxForwardSpeed                    << "m/s"   << endl
            << "     maxReverseSpeed:           " << v->maxReverseSpeed                    << "m/s"   << endl
            << "     maxTurnrate:               " << RAD2DEG(v->maxTurnrate)               << "deg/s" << endl
            << "     maxLateralAcceleration:    " << v->maxLateralAcceleration             << "m/s/s" << endl
            << "     maxForwardAcceleration:    " << v->maxForwardAcceleration             << "m/s/s" << endl
            << "     maxReverseAcceleration:    " << v->maxReverseAcceleration             << "m/s/s" << endl
            << "     maxRotationalAcceleration: " << RAD2DEG(v->maxRotationalAcceleration) << "deg/s/s";
        }
    }
    else if ( obj->ice_isA( "::orca::VehicleControlVelocityBicycleDescription" ) )
    {
        orca::VehicleControlVelocityBicycleDescriptionPtr v = orca::VehicleControlVelocityBicycleDescriptionPtr::dynamicCast( obj );
        if ( v )
        {        
            s << endl << "VehicleControlVelocityBicycleDescription: " << endl
            << "     maxForwardSpeed:           " << v->maxForwardSpeed                    << "m/s"   << endl
            << "     maxReverseSpeed:           " << v->maxReverseSpeed                    << "m/s"   << endl
            << "     maxSteerAngle:             " << RAD2DEG(v->maxSteerAngle)             << "deg" << endl
            << "     maxSteerAngleAtMaxSpeed:   " << RAD2DEG(v->maxSteerAngleAtMaxSpeed)   << "deg" << endl
            << "     maxForwardAcceleration:    " << v->maxForwardAcceleration             << "m/s"   << endl
            << "     maxReverseAcceleration:    " << v->maxReverseAcceleration             << "m/s"   << endl
            << "     maxSteerAngleRate:         " << RAD2DEG(v->maxSteerAngleRate)         << "deg/s";
        }
    }
    return s.str();
}

std::string 
toString( const orca::VehicleGeometryDescriptionPtr& obj )
{
    if ( obj == 0 )
        throw hydroutil::Exception( ERROR_INFO, "VehicleGeometryDescriptionPtr was zero!" );

    std::ostringstream s;
    s << "VehicleGeometryDescription: "
      << "type=";
    if ( obj->type == orca::VehicleGeometryCuboid )
        s << "VehicleGeometryCuboid";
    else if ( obj->type == orca::VehicleGeometryCylindrical )
        s << "VehicleGeometryCylindrical";
    else if ( obj->type == orca::VehicleGeometryOther )
        s << "VehicleGeometryOther";
    else
        s << "Unknown!!";

    if ( obj->ice_isA( "::orca::VehicleGeometryCylindricalDescription" ) )
    {
        orca::VehicleGeometryCylindricalDescriptionPtr v = orca::VehicleGeometryCylindricalDescriptionPtr::dynamicCast( obj );
        if ( v )
        { 
            s << endl << "VehicleGeometryCylindricalDescription: " << endl
            << "  radius: " << v->radius << "m" << endl
            << "  height: " << v->height << "m" << endl
            << "  vehicleToGeometryTransform: " << toString(v->vehicleToGeometryTransform);
        }
    }
    else if ( obj->ice_isA( "::orca::VehicleGeometryCuboidDescription" ) )
    {
        orca::VehicleGeometryCuboidDescriptionPtr v = orca::VehicleGeometryCuboidDescriptionPtr::dynamicCast( obj );
        if ( v )
        { 
            s << endl << "VehicleGeometryCuboidDescription: " << endl
            << "  length: " << v->size.l << "m" << endl
            << "  width:  " << v->size.w << "m" << endl
            << "  height: " << v->size.h << "m" << endl
            << "  vehicleToGeometryTransform: " << toString(v->vehicleToGeometryTransform);
        }
    }
    return s.str();
}

std::string
toString( const orca::Twist2d &t )
{
    std::ostringstream s;
    s << " Twist2d: (v->x,v->y,w): ("
      << t.v.x << ", "
      << t.v.y << ", "
      << t.w*180.0/M_PI << "deg)";
    return s.str();
}

std::string
toString( const orca::Covariance2d &cov )
{
    stringstream ss;
    ss << "("
       << cov.xx << ","
       << cov.xy << ","
       << cov.yy << ","
       << cov.xt << ","
       << cov.yt << ","
       << cov.tt*180.0/M_PI << "deg)" << endl;
    return ss.str();
}

std::string 
toString( const orca::Localise2dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " Localise2d ["<<obj.hypotheses.size()<<" elements] [weight] (x,y,a(deg) (pxx,pxy,pyy,pxt,pyt,pyy(deg)) :"<<endl;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        const orca::Pose2dHypothesis &h = obj.hypotheses[i];
        s << "  " << i << ": [" 
          << h.weight << "] (" 
          << h.mean.p.x << ","
          << h.mean.p.y << ","
          << h.mean.o*180.0/M_PI << "deg) "
          << toString(h.cov) << endl;
    }
    return s.str();
}

std::string 
toString( const orca::Localise3dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " Localise3d ["<<obj.hypotheses.size()<<" elements] [weight] (x,y,z,r(deg),p(deg),a(deg) (pxx,pxy,pxz,pyy,pyz,pzz,pxr,pxp,pxa,pyr,pyp,pya,pzr,pzp,pza,prr,prp,pra,ppp,ppa,paa(deg)) :"<<endl;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        const orca::Pose3dHypothesis &h = obj.hypotheses[i];
        s << "  " << i << ": [" 
          << h.weight << "] (" 
          << setprecision(15)
          << h.mean.p.x << ","
          << h.mean.p.y << ","
          << h.mean.p.z << ","
          << h.mean.o.r*180.0/M_PI << "deg, "
          << h.mean.o.p*180.0/M_PI << "deg, "
          << h.mean.o.y*180.0/M_PI << "deg) ("
          << h.cov.xx << ","
          << h.cov.xy << ","
          << h.cov.xz << ","
          << h.cov.yy << ","
          << h.cov.yz << ","
          << h.cov.zz << ","
          << h.cov.xr << ","
          << h.cov.xp << ","
          << h.cov.xa << ","
          << h.cov.yr << ","
          << h.cov.yp << ","
          << h.cov.ya << ","
          << h.cov.zr << ","
          << h.cov.zp << ","
          << h.cov.za << ","
          << h.cov.rr*180.0/M_PI << "deg, "
          << h.cov.rp*180.0/M_PI << "deg, "
          << h.cov.ra*180.0/M_PI << "deg, "
          << h.cov.pp*180.0/M_PI << "deg, "
          << h.cov.pa*180.0/M_PI << "deg, "
          << h.cov.aa*180.0/M_PI << "deg)" << endl;
    }
    return s.str();
}

std::string 
toString( const orca::VelocityControl2dData& obj )
{
    std::ostringstream s;
    s <<   "VelocityControl2dData (vx,vy,w(deg/s)) : ("
            << obj.motion.v.x << ", "
            << obj.motion.v.y << ", "
            << RAD2DEG(obj.motion.w) << ")";

    return s.str();
}

std::string 
toString( const orca::HomeData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp) << " HomeData"<<endl;

    IceUtil::Time t = IceUtil::Time::seconds(obj.timeUp);
    s << "time up="<<t.toDuration()<<endl;

    s << "provides={";
    for ( unsigned int i=0; i<obj.comp.provides.size(); ++i ) {
        if ( i>0) { s<<":"; }
        s << obj.comp.provides[i].name;
    }
    s << "}"<<endl;

    s << "requires={";
    for ( unsigned int i=0; i<obj.comp.requires.size(); ++i ) {
        if ( i>0) { s<<":"; }
        s << toString( obj.comp.requires[i].name );
    }
    s << "}" << endl;

    return s.str();
}

std::string 
toString( const orca::ImuData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " ImuData: \n"
        << " Accelerometer (x,y,z): "
        << obj.accel.x << " "
        << obj.accel.y << " "
        << obj.accel.z << " " << "\n"
        << " Gyro (x,y,z): "
        << obj.gyro.x << " "
        << obj.gyro.y << " "
        << obj.gyro.z << " ";
    
    return s.str();
}

std::string 
toString( const orca::LaserScanner2dDataPtr & obj, int skip )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " LaserData [" << obj->ranges.size() << " elements]" << endl;

    s << "minRange=" << obj->minRange << "m" << endl;
    s << "maxRange=" << obj->maxRange << "m" << endl;
    s << "fieldOfView=" << obj->fieldOfView * 180.0/M_PI << "deg" << endl;
    s << "startAngle=" << obj->startAngle * 180.0/M_PI << "deg" << endl;

    if ( skip > -1 ) 
    {
        s << ": (";
        for ( unsigned int i=0; i < obj->ranges.size(); ++i ) {
            s << obj->ranges[i] << ":" << (int) obj->intensities[i] << " ";
            i = i + skip;
        }
        s << ")";
        s << endl;
    }
    return s.str();
}

std::string 
toString( const orca::RangeScanner2dDataPtr & obj, int skip )
{
    std::ostringstream s;
    s << toString(obj->timeStamp)
      << " RangeScanner2dData [" << obj->ranges.size() << " elements]: " << endl;

    s << "minRange=" << obj->minRange << "m" << endl;
    s << "maxRange=" << obj->maxRange << "m" << endl;
    s << "fieldOfView=" << obj->fieldOfView * 180.0/M_PI << "deg" << endl;
    s << "startAngle=" << obj->startAngle * 180.0/M_PI << "deg" << endl;

    if ( skip > -1 ) 
    {
        s << ": (";
        for ( unsigned int i=0; i < obj->ranges.size(); ++i ) {
            s << obj->ranges[i] << " ";
            i = i + skip;
        }
        s << ")";
        s << endl;
    }
    return s.str();
}

std::string 
toString( const orca::BatteryData& obj )
{
    std::ostringstream s;
    s << "[" 
      <<obj.name<<","
      <<obj.voltage<<"V,"
      <<obj.percent<<"%,";
    if (obj.isBatteryCharging==orca::ChargingYes) {
        s << "Yes,";
    } else if (obj.isBatteryCharging==orca::ChargingNo) {
        s << "No,";
    } else {
        s << "Unknown";
    }
    s<<obj.secRemaining<<"s]";
    return s.str();
}

std::string 
toString( const orca::BatteriesData& obj )
{
    std::ostringstream s;
    s << "(name,volt,%,isCharging,sec) :" << endl;
    for ( unsigned int i=0; i < obj.size(); ++i )
    {
        s << "  " << toString(obj[i]) << endl;
    }
    return s.str();
}

std::string 
toString( const orca::PowerData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp) << " " << endl;
    s << toString(obj.batteries);
    return s.str();
}

std::string 
toString( const orca::StatusData& obj )
{
    std::ostringstream s;
    s << "Status for " << toString( obj.name )  << endl
      << "time up = " << IceUtil::Time::seconds(obj.timeUp).toDuration() << endl
      << "subsystems [" << obj.subsystems.size() << "] (name, status, msg, since beat) :";

    string type;

    for ( orca::SubsystemsStatus::const_iterator it=obj.subsystems.begin(); it!=obj.subsystems.end(); ++it ) 
    {
        switch ( it->second.type ) {
        case orca::SubsystemStatusInitialising :
            type = "INITIALISING";
            break;
        case orca::SubsystemStatusOk :
            type = "OK";
            break;
        case orca::SubsystemStatusWarning :
            type = "WARN";
            break;
        case orca::SubsystemStatusFault :
            type = "FAULT";
            break;
        case orca::SubsystemStatusStalled :
            type = "STALLED";
            break;
        }

        s << endl << "[" 
          <<it->first<<", "
          <<type<<", "
          <<it->second.message<<", "
          <<it->second.sinceHeartbeat*100.0<<"%]";
    }

    return s.str();
}

std::string 
toString( const orca::TracerData& obj )
{
    std::string s;

    s = "[ ";
    s += toString( obj.timeStamp ) + " ";
    s += toString( obj.name );
    s += " " + obj.category + ": ";
    s += obj.message + " ]";

    // replace line breaks with spaces
    string::size_type idx = 0;
    while((idx = s.find("\n", idx)) != string::npos)
    {
        s.insert(idx + 1, "  ");
        ++idx;
    }

    return s;
}

std::string 
toString( const orca::RangeScanner2dDescription& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
        << " RangeScanner2dDescription: \n"
        << "minRange=" << obj.minRange << "m\n"
        << "maxRange=" << obj.maxRange << "m\n"
        << "fieldOfView=" << RAD2DEG(obj.fieldOfView) << "deg\n"
        << "startAngle=" << RAD2DEG(obj.startAngle) << "deg\n"
        << "numberOfSamples=" << obj.numberOfSamples << "\n"
        << "offset.point.x=" << obj.offset.p.x << "m\n"
        << "offset.point.y=" << obj.offset.p.y << "m\n"
        << "offset.point.z=" << obj.offset.p.z << "m\n"
        << "offset.orientation.roll=" << RAD2DEG(obj.offset.o.r) << "deg\n"
        << "offset.orientation.pitch=" << RAD2DEG(obj.offset.o.p) << "deg\n"
        << "offset.orientation.yaw=" << RAD2DEG(obj.offset.o.y) << "deg\n"
        << "size.length=" << obj.size.l << "m\n"
        << "size.width=" << obj.size.w << "m\n"
        << "size.height=" << obj.size.h << "m";

    return s.str();
}

std::string 
toString( const orca::ImuDescription& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
            << " ImuDescription: \n"
            << "Size: \n"
            << "size.l:           " << obj.size.l << "m\n"
            << "size.w:           " << obj.size.w << "m\n"
            << "size.h:           " << obj.size.h << "m\n"
            << "Offset: \n"
            << "origin.p.x:           " << obj.offset.p.x << "m\n"
            << "offset.p.y:           " << obj.offset.p.y << "m\n"
            << "offset.p.z:           " << obj.offset.p.z << "m\n"
            << "offset.o.r:           " << obj.offset.o.r << "m\n"
            << "offset.o.p:           " << obj.offset.o.p << "m\n"
            << "offset.o.y:           " << obj.offset.o.y << "m\n";

    return s.str();
}

std::string 
toString( const orca::SinglePolarFeature2dPtr& obj )
{
    std::ostringstream s;

    if ( obj == 0 )
    {
        s << "ERROR: Feature was NULL!" << endl;
        return s.str();
    }

    s << "(t" << obj->type
      << ",pf" << obj->pFalsePositive
      << ",pt" << obj->pTruePositive
      << ")";

    // a bit ugly...
    if ( obj->ice_isA( "::orca::PointPolarFeature2d" ) )
    {
        const orca::PointPolarFeature2d& f = dynamic_cast<const orca::PointPolarFeature2d&>(*obj);
        s << "(r="<<f.p.r<<",b="<<f.p.o*180.0/M_PI << "deg)";
    }
    else if ( obj->ice_isA( "::orca::LinePolarFeature2d" ) )
    {
        const orca::LinePolarFeature2d& f = dynamic_cast<const orca::LinePolarFeature2d&>(*obj);
        s << "(r="<<f.start.r<<",b="<<f.start.o*180.0/M_PI << "deg -> r="
          <<f.end.r<<",b="<<f.end.o*180.0/M_PI << "deg)";
    }
    else
    {
        s << "(generic)" << endl;
    }
    return s.str();
}

std::string 
toString( const orca::PolarFeature2dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " PolarFeatureData [" << obj.features.size() << " elements] (type,pFalsePos,pTruePos) (details): " << endl;

    const orca::PolarFeature2dSequence &features = obj.features;
    for (unsigned int i=0; i < obj.features.size(); i++)
    {
        s << "  " << i << ": " << toString(features[i]) << endl;
    }
    s << endl;
    
    return s.str();
}

std::string 
toString( const orca::OgMapData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " OgMap: \n"
      << "offset:        [" << obj.offset.p.x << ", " << obj.offset.p.y << ", " << obj.offset.o*180.0/M_PI << "]\n"
      << "numCells:      [" << obj.numCellsX << ", " << obj.numCellsY << "]\n"
      << "metresPerCell: [" << obj.metresPerCellX << ", " << obj.metresPerCellY << "]\n";

    return s.str();
}

std::string 
toString( const orca::PixMapData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " PixMap: \n"
      << "offset:        [" << obj.offset.p.x << ", " << obj.offset.p.y << ", " << obj.offset.o*180.0/M_PI << "]\n"
      << "numCells:      [" << obj.numCellsX << ", " << obj.numCellsY << "]\n"
      << "metresPerCell: [" << obj.metresPerCellX << ", " << obj.metresPerCellY << "]\n";

    return s.str();
}

std::string 
toString( const orca::PathFollower2dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " PathFollower2dData: " << obj.path.size() << " waypoints.";
    return s.str();
}

std::string 
toString( const orca::Waypoint2d &obj )
{
    std::ostringstream s;
    s << "target=" << toString(obj.target) 
      << ", tolerances=[" << obj.distanceTolerance << "," << obj.headingTolerance*180.0/M_PI << "deg]"
      << ", timeTarget=[" << toString(obj.timeTarget) << "]"
      << ", maxApproach=[" << obj.maxApproachSpeed << "m/s," << obj.maxApproachTurnrate*180.0/M_PI << "deg/s]";
    return s.str();
}

std::string
toString( const orca::LinkQualityType &l )
{
    switch ( l )
    {
    case orca::LinkQualityTypeUnknown:
        return "LinkQualityTypeUnknown";
    case orca::LinkQualityTypeDbm:
        return "LinkQualityTypeDbm";
    case orca::LinkQualityTypeRelative:
        return "LinkQualityTypeRelative";
    }
    throw hydroutil::Exception( ERROR_INFO, "Don't know how to handle this type in toString()" );
}

std::string
toString( const orca::OperationMode &o )
{
    switch ( o )
    {
    case orca::OperationModeUnknown:
        return "OperationModeUnknown";
    case orca::OperationModeAuto:
        return "OperationModeAuto";
    case orca::OperationModeAdhoc:
        return "OperationModeAdhoc";
    case orca::OperationModeInfrastructure:
        return "OperationModeInfrastructure";
    case orca::OperationModeMaster:
        return "OperationModeMaster";
    case orca::OperationModeRepeat:
        return "OperationModeRepeat";
    case orca::OperationModeSecondRepeater:
        return "OperationModeSecondRepeater";
    }
    throw hydroutil::Exception( ERROR_INFO, "Don't know how to handle this type in toString()" );
}

std::string 
toString( const orca::WifiData &obj )
{
    std::ostringstream s;
    // first line: timestamp
    s << toString(obj.timeStamp) << " WifiData [" << obj.interfaces.size() << " interfaces]: " << endl;
    
    for ( unsigned int i=0; i<obj.interfaces.size(); ++i )
    {
        const orca::WifiInterface &iface = obj.interfaces[i];
        
        s << "name: " << iface.interfaceName 
          << ", status: " << iface.status 
          << ", link quality: " <<  iface.linkQuality 
          << ", signal level: " << iface.signalLevel 
          << ", noise level: " << iface.noiseLevel << "\n"; 
        
        s << "numInvalidNwid: " << iface.numInvalidNwid
          << ", numInvalidCrypt: " << iface.numInvalidCrypt
          << ", numInvalidFrag: " << iface.numInvalidFrag
          << ", numRetries: " << iface.numRetries
          << ", numInvalidMisc: " << iface.numInvalidMisc
          << ", numMissedBeacons: " << iface.numMissedBeacons << "\n";
          
        s << "mode: " << toString(iface.mode) 
          << ", bitrate: " << iface.bitrate 
          << ", accessPoint: " << iface.accessPoint 
          << ", throughPut: " << iface.throughPut 
          << ", linkQualityType: " << toString(iface.linkType) << "\n";
        
        s << "maxLinkQuality: " << iface.maxLinkQuality 
          << ", maxSignalLevel: " << iface.maxSignalLevel 
          << ", maxNoiseLevel: " << iface.maxNoiseLevel << "\n\n";
    }
            
    return s.str();
}

std::string
toString( const orca::PropertiesData &obj )
{
    std::ostringstream s;
    s << " PropertiesData ["<<obj.properties.size() << " elements]:"<<endl;
    for ( map<string,string>::const_iterator it = obj.properties.begin();
          it != obj.properties.end();
          it++ )
    {
        s << "  " << it->first << "=" << it->second << endl;
    }
    return s.str();
}

std::string 
toString( const orca::FeatureMap2dData &obj )
{
    std::ostringstream s;

    s << " FeatureMap2d ["<<obj.features.size()<<" elements]: "<<endl;

    for (unsigned int i=0; i < obj.features.size(); i++)
    {
        assert( obj.features[i] != 0 );

        const orca::Feature2dPtr &f = obj.features[i];
        s << "  " << i << ": " << toString(*f) << endl;
    }    
    s << endl;
    
    return s.str();
}

std::string 
toString( const orca::Feature2d &f )
{
    std::ostringstream s;

    //
    // I'm not convinced that this is the best way of doing things...
    //
    s << "[t"<<f.type<<",p"<<f.pExists<<"]";

    if ( f.ice_isA( "::orca::CartesianPointFeature2d" ) )
    {
        const orca::CartesianPointFeature2d& r = dynamic_cast<const orca::CartesianPointFeature2d&>(f);
        s << "("<<r.p.x<<","<<r.p.y<<") ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")";
    }
    else if ( f.ice_isA( "::orca::CartesianPoseFeature2d" ) )
    {
        const orca::CartesianPoseFeature2d& r = dynamic_cast<const orca::CartesianPoseFeature2d&>(f);
        s << "("<<r.p.p.x<<","<<r.p.p.y<<","<<r.p.o*180.0/M_PI<<") ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")";
    }
    else if ( f.ice_isA( "::orca::CartesianLineFeature2d" ) )
    {
        const orca::CartesianLineFeature2d& r = dynamic_cast<const orca::CartesianLineFeature2d&>(f);
        s << "(r="<<r.rho<<",a="<<r.alpha*180.0/M_PI<<"deg) ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")" << " ("<<r.start.x<<","<<r.start.y<<")->("<<r.end.x<<","<<r.end.y<<")";
    }
    else
    {
        // Don't really know how to display info about this feature.
        assert( false );
    }
    
    return s.str();
}

//**************************
// toVerboseString()
//*************************

std::string 
toVerboseString( const orca::OgMapData& obj )
{
    std::stringstream ss;
    int szX=obj.numCellsX;
    int szY=obj.numCellsY;
    int x, y;
    int step = 1 + obj.numCellsX / 200;

    ss<<endl;
    ss<<"      ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    for( y=szY-1; y>=0; y-=step)
    {
        ss<<"     | ";
        for( x=0; x<szX; x+=step)
        {
            ss<<displayOgmapCell( obj.data[ gridCellIndex(obj,x,y) ] );
        }
        ss<<"|"<<endl;
    }

    ss<<"  y   ";
    for( x=0; x<szX; x+=step )
    {
        ss<<"-";
    }
    ss<<"-"<<endl;

    ss<<"  |"<<endl;
    ss<<"  |____ x"<<endl;
    return ss.str();
}

std::string 
toVerboseString( const orca::PathFollower2dData& obj )
{
    std::stringstream s;

    s << orcaice::toString(obj.timeStamp)
      << " PathFollower2dData [" << obj.path.size() << " waypoints]:" << endl;
    for ( unsigned int i=0; i < obj.path.size(); i++ )
    {
        s << "  " << i << ":" << orcaice::toString(obj.path[i]) << endl;
    }

    return s.str();
}

std::string
toString( const orca::PathPlanner2dResult &res )
{
    switch ( res )
    {
    case orca::PathOk:
        return "PathOk";
    case orca::PathStartNotValid:
        return "PathStartNotValid";
    case orca::PathDestinationNotValid:
        return "PathDestinationNotValid";
    case orca::PathDestinationUnreachable:
        return "PathDestinationUnreachable";
    case orca::PathOtherError:
        return "PathOtherError";
    }
    return "";
}

std::string 
toVerboseString( const orca::PathPlanner2dData& obj )
{
    std::stringstream s;

    s << obj.timeStamp.seconds << "s:" << obj.timeStamp.useconds << "us"
      << " PathPlanner2dDataPtr [" << obj.path.size() << " waypoints. Result code: " << toString(obj.result) << " ]:" << endl;
    s << "Result description: " << obj.resultDescription << endl;
    for ( unsigned int i=0; i < obj.path.size(); i++ )
    {
        s << "  " << i << ":" << orcaice::toString(obj.path[i]) << endl;
    }

    return s.str();
}

std::string 
toVerboseString( const orca::PathPlanner2dTask& obj )
{
    std::stringstream s;

    s << obj.timeStamp.seconds << "s:" << obj.timeStamp.useconds << "us"
      << " PathPlanner2dTaskPtr [path contains "<< obj.coarsePath.size() << " waypoints]:" << endl;
    for ( unsigned int i=0; i < obj.coarsePath.size(); i++ )
    {
        s << "  " << i << ":" << orcaice::toString(obj.coarsePath[i]) << endl;
    }

    return s.str();
}

std::string 
toVerboseString( const orca::ImuData& obj )
{
    std::stringstream s;

    s << orcaice::toString(obj.timeStamp)
        << " ImuData: \n"
        << "Accelerometer (x,y,z): "
        << obj.accel.x << " "
        << obj.accel.y << " "
        << obj.accel.z << " \n"
        << "Gyro (x,y,z): "
        << obj.gyro.x << " "
        << obj.gyro.y << " "
        << obj.gyro.z << " ";

    return s.str();
}

} // namespace
