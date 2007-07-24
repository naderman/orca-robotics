/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <orcaobj/timeutils.h>
#include <orcaobj/miscutils.h>

#include "logstringutils.h"

// this is a copy from orcaice/mathdefs.h
// make a copy here to cut circular dependency
//==============
#ifndef DEG2RAD_RATIO
#define DEG2RAD_RATIO	(M_PI/180.0)
#endif

//! Converts from degrees to radians.
#define DEG2RAD(deg)	((deg)*DEG2RAD_RATIO)
//! Converts from radians to degrees.
#define RAD2DEG(rad) 	((rad)/DEG2RAD_RATIO)
//==============

using namespace std;

namespace orcalog
{

namespace {
//////////////////////////////////////////////////////////////////////
// Generic utilities

    // These guys just make the code below a little more readable.
    inline void fromLogString( std::stringstream &s, int &i )
    { s >> i; }
    inline void fromLogString( std::stringstream &s, double &i )
    { s >> i; }


    std::string toLogString( const orca::CartesianPoint &o )
    {
        stringstream ss;
        ss << o.x << " " << o.y << " " << o.z;
        return ss.str();
    }
    void fromLogString( std::stringstream &s, orca::CartesianPoint &o )
    {
        s >> o.x >> o.y >> o.z;
    }

    std::string toLogString( const orca::OrientationE &o )
    {
        stringstream ss;
        ss << o.r << " " << o.p << " " << o.y;
        return ss.str();
    }
    void fromLogString( std::stringstream &s, orca::OrientationE &o )
    {
        s >> o.r >> o.p >> o.y;
    }

    std::string toLogString( const orca::Frame3d &o )
    {
        stringstream ss;
        ss << toLogString(o.p) << " " << toLogString(o.o);
        return ss.str();
    }
    void fromLogString( std::stringstream &s, orca::Frame3d &o )
    {
        fromLogString( s, o.p );
        fromLogString( s, o.o );
    }

    std::string toLogString( const orca::CartesianPoint2d &o )
    {
        stringstream ss;
        ss << o.x << " " << o.y;
        return ss.str();
    }
    void fromLogString( std::stringstream &s, orca::CartesianPoint2d &o )
    {
        s >> o.x >> o.y;
    }

    std::string toLogString( const orca::Frame2d &o )
    {
        stringstream ss;
        ss << toLogString(o.p) << " " << o.o;
        return ss.str();
    }
    void fromLogString( std::stringstream &s, orca::Frame2d &o )
    {
        fromLogString( s, o.p );
        s >> o.o;
    }

//////////////////////////////////////////////////////////////////////
}

std::string 
toLogString( const orca::Waypoint2d& obj )
{
    // waypoint angles are specified in 1/16 of a degree from the GUI.
    // For consistentcy, log them from 0 to 360*16
    std::ostringstream s;
    s << obj.target.p.x << " " << obj.target.p.y << " " << obj.target.o*16*180.0/M_PI << " " 
      << obj.timeTarget.seconds + obj.timeTarget.useconds/1000000.0 << " " 
      << obj.distanceTolerance << " " 
      << obj.headingTolerance*16*180.0/M_PI << " "
      << obj.maxApproachSpeed << " "
      << obj.maxApproachTurnrate*16*180.0/M_PI
      << endl;
    return s.str();
}

std::string 
toLogString( const orca::CpuData& obj )
{
    std::ostringstream s;
    s << toLogString(obj.timeStamp)
        << " " << obj.userLoad
        << " " << obj.systemLoad
        << " " << obj.idleLoad
        << " " << obj.temperature
        << endl;
    return s.str();
}

int convertToNumeric( orca::GpsPositionType p )
{
    switch ( p )
    {
    case orca::GpsPositionTypeNotAvailable:
        return 0;
    case orca::GpsPositionTypeAutonomous:
        return 1;
    case orca::GpsPositionTypeDifferential:
        return 2;
    case orca::NovatelNone:
        return 3;
    case orca::NovatelFixedPos:
        return 4;
    case orca::NovatelFixedHeigth:
        return 5;
    case orca::NovatelFloatConv:
        return 6;
    case orca::NovatelWideLane:
        return 7;
    case orca::NovatelNarrowLane:
        return 8;
    case orca::NovatelDopplerVelocity:
        return 9;
    case orca::NovatelSingle:
        return 10;
    case orca::NovatelPsrDiff:
        return 11;
    case orca::NovatelWAAS:
        return 12;
    case orca::NovatelPropagated:
        return 13;
    case orca::NovatelOmnistar:
        return 14;
    case orca::NovatelL1Float:
        return 15;
    case orca::NovatelIonFreeFloat:
        return 16;
    case orca::NovatelNarrowFloat:
        return 17;
    case orca::NovatelL1Int:
        return 18;
    case orca::NovatelWideInt:
        return 19;
    case orca::NovatelNarrowInt:
        return 20;
    case orca::NovatelRTKDirectINS:
        return 21;
    case orca::NovatelINS:
        return 22;
    case orca::NovatelINSPSRSP:
        return 23;
    case orca::NovatelINSPSRFLOAT:
        return 24;
    case orca::NovatelINSRTKFLOAT:
        return 25;
    case orca::NovatelINSRTKFIXED:
        return 26;
    case orca::NovatelOmnistarHP:
        return 27;
    default:
        return -1;
    }
}

orca::GpsPositionType convertFromNumeric( int p )
{
    switch ( p )
    {
    case 0:
        return orca::GpsPositionTypeNotAvailable;
    case 1:
        return orca::GpsPositionTypeAutonomous;
    case 2:
        return orca::GpsPositionTypeDifferential;
    case 3:
        return orca::NovatelNone;
    case 4:
        return orca::NovatelFixedPos;
    case 5:
        return orca::NovatelFixedHeigth;
    case 6:
        return orca::NovatelFloatConv;
    case 7:
        return orca::NovatelWideLane;
    case 8:
        return orca::NovatelNarrowLane;
    case 9:
        return orca::NovatelDopplerVelocity;
    case 10:
        return orca::NovatelSingle;
    case 11:
        return orca::NovatelPsrDiff;
    case 12:
        return orca::NovatelWAAS;
    case 13:
        return orca::NovatelPropagated;
    case 14:
        return orca::NovatelOmnistar;
    case 15:
        return orca::NovatelL1Float;
    case 16:
        return orca::NovatelIonFreeFloat;
    case 17:
        return orca::NovatelNarrowFloat;
    case 18:
        return orca::NovatelL1Int;
    case 19:
        return orca::NovatelWideInt;
    case 20:
        return orca::NovatelNarrowInt;
    case 21:
        return orca::NovatelRTKDirectINS;
    case 22:
        return orca::NovatelINS;
    case 23:
        return orca::NovatelINSPSRSP;
    case 24:
        return orca::NovatelINSPSRFLOAT;
    case 25:
        return orca::NovatelINSRTKFLOAT;
    case 26:
        return orca::NovatelINSRTKFIXED;
    case 27:
        return orca::NovatelOmnistarHP;
    default:
        return orca::NovatelUnknown;
    }
}

std::string 
toLogString( const orca::GpsData& obj )
{
    int positionType = convertToNumeric( obj.positionType );
    
    std::ostringstream s;
    s << toLogString(obj.timeStamp) << " "
      << obj.utcTime.hours << " "
      << obj.utcTime.minutes << " "
      << obj.utcTime.seconds << " "
      << setprecision(16)
      << obj.latitude << " "
      << obj.longitude << " "
      << setprecision(4)
      << obj.altitude << " "
      << obj.horizontalPositionError<< " "
      << obj.verticalPositionError << " "
      << obj.heading << " "
      << obj.speed << " "
      << obj.climbRate << " "
      << obj.satellites << " "
      << obj.observationCountOnL1 << " "
      << obj.observationCountOnL2 << " "
      << positionType << " "
      << obj.geoidalSeparation;
    return s.str();
}

void
fromLogString( std::stringstream &s, orca::GpsData& obj )
{
    fromLogString( s, obj.timeStamp );
    fromLogString( s, obj.utcTime.hours );
    fromLogString( s, obj.utcTime.minutes );
    fromLogString( s, obj.utcTime.seconds );
    fromLogString( s, obj.latitude );
    fromLogString( s, obj.longitude );
    fromLogString( s, obj.altitude );
    fromLogString( s, obj.horizontalPositionError );
    fromLogString( s, obj.verticalPositionError );
    fromLogString( s, obj.heading );
    fromLogString( s, obj.speed );
    fromLogString( s, obj.climbRate );
    fromLogString( s, obj.satellites );
    fromLogString( s, obj.observationCountOnL1 );
    fromLogString( s, obj.observationCountOnL2 );
    int numericPosType;
    fromLogString( s, numericPosType );
    obj.positionType = convertFromNumeric( numericPosType );
    fromLogString( s, obj.geoidalSeparation );
}

std::string 
toLogString( const orca::GpsTimeData& obj )
{
    std::ostringstream s;
    s << toLogString(obj.timeStamp) << " "
      << obj.utcTime.hours << " "
      << obj.utcTime.minutes << " "
      << obj.utcTime.seconds << " "
      << obj.utcDate.day << " "
      << obj.utcDate.month << " "
      << obj.utcDate.year;
    return s.str();
}

void
fromLogString( std::stringstream &s, orca::GpsTimeData& obj )
{
    fromLogString( s, obj.timeStamp );
    fromLogString( s, obj.utcTime.hours );
    fromLogString( s, obj.utcTime.minutes );
    fromLogString( s, obj.utcTime.seconds );
    fromLogString( s, obj.utcDate.day );
    fromLogString( s, obj.utcDate.month );
    fromLogString( s, obj.utcDate.year );
}

std::string 
toLogString( const orca::GpsMapGridData& obj )
{
    int positionType = convertToNumeric( obj.positionType );
    
    std::ostringstream s;
    s << toLogString(obj.timeStamp) << " "
      << obj.utcTime.hours << " "
      << obj.utcTime.minutes << " "
      << obj.utcTime.seconds << " "
      << obj.northing << " "
      << obj.easting << " "
      << obj.altitude << " "
      << obj.horizontalPositionError<< " "
      << obj.verticalPositionError << " "
      << obj.heading << " "
      << obj.speed << " "
      << obj.climbRate << " "
      << obj.zone << " "
      << positionType;
    return s.str();
}

void
fromLogString( std::stringstream &s, orca::GpsMapGridData& obj )
{
    fromLogString( s, obj.timeStamp );
    fromLogString( s, obj.utcTime.hours );
    fromLogString( s, obj.utcTime.minutes );
    fromLogString( s, obj.utcTime.seconds );
    fromLogString( s, obj.northing );
    fromLogString( s, obj.easting );
    fromLogString( s, obj.altitude );
    fromLogString( s, obj.horizontalPositionError );
    fromLogString( s, obj.verticalPositionError );
    fromLogString( s, obj.heading );
    fromLogString( s, obj.speed );
    fromLogString( s, obj.climbRate );
    fromLogString( s, obj.zone );
    int numericPosType;
    fromLogString( s, numericPosType );
    obj.positionType = convertFromNumeric( numericPosType );
}

std::string 
toLogString( const orca::LaserScanner2dDataPtr& obj )
{
    std::stringstream s;

    // time stamp, start angle, and number of points in one laser scan
    // separated by spaces on the 1st line
    s << toLogString(obj->timeStamp) << " ";
    s << RAD2DEG( obj->startAngle ) << " ";
    s << obj->ranges.size() << " \n";

    // ranges on the second line
    for ( unsigned int i=0; i < obj->ranges.size(); i++ )
    {
        s << obj->ranges[i] << " ";
    }
    s << "\n";
    // intensites on the third line
    for ( unsigned int i=0; i < obj->ranges.size(); i++ )
    {
	   s << (float)obj->intensities[i] << " ";
    }

    return s.str();
}

/*! 
@brief Prints out localise2d data to text which is easy to parse.

- line1: timestamp in seconds and microseconds of Unix time
- line2: number of hypotheses (N)
- line3: hypothesis 1: {means covariances weight}. For element order see below.
- ...
- line2+N: hypothesis N

The state is written in this order: {x y yaw}

The covariance matrix is written in this order, e.i. {xx xy yy xyaw yyaw yawyaw}

Units: means are in [m], angles are in [deg].
*/
std::string 
toLogString( const orca::Localise2dData& obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj.timeStamp) << endl;

    // number of hypotheses on a line
    s << obj.hypotheses.size() << endl;

    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        const orca::Pose2dHypothesis &h = obj.hypotheses[i];

        s << h.mean.p.x << " "
          << h.mean.p.y << " "
          << RAD2DEG(h.mean.o) << " "
          << h.cov.xx << " "
          << h.cov.xy << " "
          << h.cov.yy << " "
          << h.cov.xt << " "
          << h.cov.yt << " "
          << h.cov.tt << " "
          << h.weight << "\n";
    }
    return s.str();
}


/*! 
@brief Prints out localise3d data to text which is easy to parse.

- line1: timestamp in seconds and microseconds of Unix time
- line2: number of hypotheses (N)
- line3: hypothesis 1: {means covariances weight}. For element order see below.
- ...
- line2+N: hypothesis N

The state is written in this order: {x y z roll pitch yaw}

The covariance matrix (its upper triangle) is written rows first, e.i. {xx xy xz xroll xpitch xyaw yy yz yroll...}

Units: means are in [m], angles are in [deg].
*/
std::string 
toLogString( const orca::Localise3dData& obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj.timeStamp) << endl;

    // number of hypotheses on a line
    s << obj.hypotheses.size() << endl;

    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        const orca::Pose3dHypothesis &h = obj.hypotheses[i];

        s << setprecision(15)
          << h.mean.p.x << " "
          << h.mean.p.y << " "
          << h.mean.p.z << " "
          << RAD2DEG(h.mean.o.r) << " "
          << RAD2DEG(h.mean.o.p) << " "
          << RAD2DEG(h.mean.o.y) << " "
          << h.cov.xx << " "
          << h.cov.xy << " "
          << h.cov.xz << " "
          << h.cov.xr << " "
          << h.cov.xp << " "
          << h.cov.xa << " "
          << h.cov.yy << " "
          << h.cov.yz << " "
          << h.cov.yr << " "
          << h.cov.yp << " "
          << h.cov.ya << " "
          << h.cov.zz << " "
          << h.cov.zr << " "
          << h.cov.zp << " "
          << h.cov.za << " "
          << h.cov.rr << " "
          << h.cov.rp << " "
          << h.cov.ra << " "
          << h.cov.pp << " "
          << h.cov.pa << " "
          << h.cov.aa << " "
          << h.weight << "\n";
    }
    return s.str();
}

/*! 
@brief Prints out Odometry2d data to text which is easy to parse.

- line1: timestamp in seconds and microseconds of Unix time
- line2: {2D pose} {2D velocities}

Components of both pose and velocities are written in this order {x y yaw}.

Units: positions are in [m], angles are in [deg], linear velocities are in [m/s], angular velocities are in [deg/s]
*/
std::string 
toLogString( const orca::Odometry2dData& obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj.timeStamp) << " \n";

    // x,y position, orientation(deg), and respective velocities
    s << obj.pose.p.x << " " 
      << obj.pose.p.y << " "
      << RAD2DEG(obj.pose.o) << " "
      << obj.motion.v.x << " "
      << obj.motion.v.y << " "
      << RAD2DEG(obj.motion.w);

    return s.str();
}

std::string 
toLogString( const orca::Odometry3dData& obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj.timeStamp) << " \n";

    // x,y,z position, roll,pitch,yaw(deg), and respective velocities on the second line
    s << setprecision(15)
      << obj.pose.p.x << " " 
      << obj.pose.p.y << " "
      << obj.pose.p.z << " "
      << RAD2DEG(obj.pose.o.r) << " "
      << RAD2DEG(obj.pose.o.p) << " "
      << RAD2DEG(obj.pose.o.y) << " \n"
      << obj.motion.v.x << " "
      << obj.motion.v.y << " "
      << obj.motion.v.z << " "
      << RAD2DEG(obj.motion.w.x) << " "
      << RAD2DEG(obj.motion.w.y) << " "
      << RAD2DEG(obj.motion.w.z);

    return s.str();
}

/*! 
@brief Prints out DriveBicycleData to text which is easy to parse.

One data structure per line.
{time stamp} {reference command} {current control state}

- time stamp
    - (int) seconds
    - (int) microseconds
- reference command
    - (double) speed [m/s]
    - (double) steer angle [deg]
- current control state
    - (double) speed [m/s]
    - (double) steer angle [deg]
*/
std::string 
toLogString( const orca::DriveBicycleData& obj )
{
    std::stringstream s;
    
    s << toLogString(obj.timeStamp) << " "
      << obj.referenceSpeed << " " 
      << RAD2DEG(obj.referenceSteerAngle) << " "
      << obj.currentSpeed << " " 
      << RAD2DEG(obj.currentSteerAngle);

    return s.str();
}

// std::string 
// toLogString( const orca::PolarFeature2dData& obj )
// {
//     std::stringstream s;

//     // timestamp on the first line
//     s << toLogString(obj.timeStamp) << " \n";

//     // feature type, range, and bearing (deg) on second line
//     const orca::PolarFeature2dSequence &features = obj.features;
//     for (unsigned int i=0; i < obj.features.size(); i++)
//     {
//         if ( features[i] == 0 )
//         {
//             s << "  " << i << ": ERROR: Feature was NULL!" << endl;
//         }
//         else
//         {
//             const orca::SinglePolarFeature2d&f = features[i];
//             s << f->type << " "
//               << f->p.r << " "
//               << f->p.o*180.0/M_PI << " ";
//         }
//     }
//     s << "\n";
    
//     return s.str();
// }

std::string
toLogString( const orca::ChargingState &c )
{
    if (c==orca::ChargingYes) return "ChargingYes";
    else if (c==orca::ChargingNo) return "ChargingNo";
    
    return "ChargingUnknown";
}

orca::ChargingState
fromLogString( const std::string &str )
{
    if (str=="ChargingYes") return orca::ChargingYes;
    else if (str=="ChargingNo") return orca::ChargingNo;
    
    return orca::ChargingUnknown;
}

// Logs only numeric data including an enum. All data is on one line.
// The number of batteries usually does not change over time.
// Format: sec usec voltage0 percent0 isBatteryCharging0 secRemaining0  ... voltageN percentN isBatteryChargingN secRemainingN
std::string 
toLogString( const orca::PowerData& obj )
{
    std::stringstream s;
    s << toLogString(obj.timeStamp);

    s << " " << obj.batteries.size();
    for ( unsigned int i=0; i < obj.batteries.size(); ++i ) {
        s << " " << obj.batteries[i].voltage << " " 
                 << obj.batteries[i].percent << " " 
                 << toLogString(obj.batteries[i].isBatteryCharging) << " "
                 << obj.batteries[i].secRemaining;
    }
    return s.str();
}

void 
fromLogString( std::stringstream& stream, orca::PowerData& obj )
{
    fromLogString( stream, obj.timeStamp );

    unsigned int size;
    stream >> size;

    obj.batteries.resize(size);
    std::string str;
    
    for ( unsigned int i=0; i < size; ++i ) {
        stream >> obj.batteries[i].voltage 
               >> obj.batteries[i].percent
               >> str
               >> obj.batteries[i].secRemaining;
        
        obj.batteries[i].isBatteryCharging = fromLogString(str);
    }
}

std::string 
toLogString( const orca::PathPlanner2dTask& obj )
{
    std::stringstream s;

    // first line: timestamp, number of waypoints
    s << toLogString(obj.timeStamp) << " " 
      << obj.coarsePath.size() << " \n";
    
    for ( unsigned int i=0; i < obj.coarsePath.size(); i++ )
    {
        s << toLogString(obj.coarsePath[i]) << " ";
    }

    return s.str();
}

std::string 
toLogString( const orca::PathPlanner2dData& obj )
{
    std::stringstream s;
    
    // first line: timestamp, result code, number of waypoints
    s << toLogString(obj.timeStamp) << " "
      << obj.result << " "
      << obj.path.size() << "\n";
    
    // second line: path
    for ( unsigned int i=0; i < obj.path.size(); i++ )
    {
        s << toLogString(obj.path[i]) << " ";
    }

    return s.str();
}


std::string 
toLogString( const orca::Time& obj )
{
    std::ostringstream s;
    s << obj.seconds << " " << obj.useconds;
    return s.str();
}
void 
        
fromLogString( std::stringstream& stream, orca::Time& obj )
{
    stream >> obj.seconds >> obj.useconds;
}

std::string 
toLogString( const orca::WifiData& obj )
{
    std::stringstream s;
    
    // first line: timestamp
    s << toLogString(obj.timeStamp) << "\n";
    
    // number of interfaces
    s << obj.interfaces.size() << "\n";
     
    // all data in a single line
    for ( unsigned int i=0; i<obj.interfaces.size(); ++i )
    {
        const orca::WifiInterface &iface = obj.interfaces[i];
        
        s << iface.interfaceName << " " 
          << iface.status << " " 
          << iface.linkQuality << " " 
          << iface.signalLevel << " " 
          << iface.noiseLevel << " " 
          << iface.numInvalidNwid << " " 
          << iface.numInvalidCrypt << " " 
          << iface.numInvalidFrag << " " 
          << iface.numRetries << " " 
          << iface.numInvalidMisc << " " 
          << iface.numMissedBeacons << " " 
          << iface.mode << " " 
          << iface.bitrate << " " 
          << iface.accessPoint << " " 
          << iface.throughPut << " " 
          << iface.linkType << " " 
          << iface.maxLinkQuality << " " 
          << iface.maxSignalLevel << " " 
          << iface.maxNoiseLevel << "\n";
    }

    return s.str();
}

std::string 
toLogString( const orca::ImuData& obj )
{
    std::stringstream s;
    
    s << toLogString(obj.timeStamp) << " "
      << obj.accel.x << " "
      << obj.accel.y << " "
      << obj.accel.z << " "
      << obj.gyro.x << " "
      << obj.gyro.y << " "
      << obj.gyro.z;
    
    return s.str();
}

std::string 
toLogString( const orca::GpsDescription& obj )
{
    std::ostringstream s;
    s << toLogString(obj.timeStamp) << " "
      << toLogString(obj.offset) << " "
      << toLogString(obj.antennaOffset);
    return s.str();
}

void 
fromLogString( std::stringstream& s, orca::GpsDescription& obj )
{
    fromLogString( s, obj.timeStamp );
    fromLogString( s, obj.offset );
    fromLogString( s, obj.antennaOffset );
}


}

