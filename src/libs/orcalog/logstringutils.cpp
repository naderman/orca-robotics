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

std::string 
toLogString( const orca::Waypoint2d& obj )
{
    std::ostringstream s;
    s << obj.target.p.x << " " << obj.target.p.y << " " << obj.target.o*180.0/M_PI << " " 
      << obj.distanceTolerance << " " 
      << obj.headingTolerance*180.0/M_PI << " "
      << toLogString(obj.timeTarget) << " " 
      << obj.maxApproachSpeed << " "
      << obj.maxApproachTurnrate*180.0/M_PI
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

std::string 
toLogString( const orca::GpsData& obj )
{
    std::ostringstream s;
    s << toLogString(obj.timeStamp) << " "
      << obj.utcTime.hours << " "
      << obj.utcTime.minutes << " "
      << obj.utcTime.seconds << " "
      << obj.latitude << " "
      << obj.longitude << " "
      << obj.altitude << " "
      << obj.heading << " "
      << obj.speed << " "
      << obj.climbRate << " "
      << obj.satellites << " "
      << obj.positionType << " "
      << obj.geoidalSeparation;
    return s.str();
}

std::string 
toLogString( const orca::GpsMapGridData& obj )
{
    std::ostringstream s;
    s << toLogString(obj.timeStamp) << " "
      << obj.utcTime.hours << " "
      << obj.utcTime.minutes << " "
      << obj.utcTime.seconds << " "
      << obj.northing << " "
      << obj.easting << " "
      << obj.altitude << " "
      << obj.heading << " "
      << obj.speed << " "
      << obj.climbRate << " "
      << obj.zone << " "
      << obj.positionType;
    return s.str();
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

std::string 
toLogString( const orca::Position2dData& obj )
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

std::string 
toLogString( const orca::Position3dData& obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj.timeStamp) << " \n";

    // x,y,z position, roll,pitch,yaw(deg), and respective velocities on the second line
    s << obj.pose.p.x << " " 
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
    s << obj.pose.p.x << " " 
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

// logs only numeric data. all data on one line
// the number of batteries usually does not change over time
// format: sec usec voltage0 percent0 voltage1 percent1 ... voltageN percentN
std::string 
toLogString( const orca::PowerData& obj )
{
    std::stringstream s;
    s << toLogString(obj.timeStamp);

    s << " " << obj.batteries.size();
    for ( unsigned int i=0; i < obj.batteries.size(); ++i ) {
        s << " " << obj.batteries[i].voltage << " " << obj.batteries[i].percent << " " << obj.batteries[i].secRemaining;
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
    for ( unsigned int i=0; i < size; ++i ) {
        stream >> obj.batteries[i].voltage >> obj.batteries[i].percent >> obj.batteries[i].secRemaining;
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

void 
fromLogString( std::stringstream& stream, orca::WifiData& obj )
{
    fromLogString( stream, obj.timeStamp );

    unsigned int size;
    stream >> size;

    obj.interfaces.resize(size);
    for ( unsigned int i=0; i < size; ++i ) 
    {
        int mode, linkType;
        
        stream  >> obj.interfaces[i].interfaceName
                >> obj.interfaces[i].status
                >> obj.interfaces[i].linkQuality 
                >> obj.interfaces[i].signalLevel
                >> obj.interfaces[i].noiseLevel
                >> obj.interfaces[i].numInvalidNwid
                >> obj.interfaces[i].numInvalidCrypt
                >> obj.interfaces[i].numInvalidFrag
                >> obj.interfaces[i].numRetries
                >> obj.interfaces[i].numInvalidMisc
                >> obj.interfaces[i].numMissedBeacons
                >> mode
                >> obj.interfaces[i].bitrate
                >> obj.interfaces[i].accessPoint 
                >> obj.interfaces[i].throughPut
                >> linkType
                >> obj.interfaces[i].maxLinkQuality 
                >> obj.interfaces[i].maxSignalLevel
                >> obj.interfaces[i].maxNoiseLevel;
           
        switch (mode)
        {
            case 0: obj.interfaces[i].mode = orca::OperationModeUnknown; break;
            case 1: obj.interfaces[i].mode = orca::OperationModeAuto; break;
            case 2: obj.interfaces[i].mode = orca::OperationModeAdhoc; break;
            case 3: obj.interfaces[i].mode = orca::OperationModeInfrastructure; break;
            case 4: obj.interfaces[i].mode = orca::OperationModeMaster; break;
            case 5: obj.interfaces[i].mode = orca::OperationModeRepeat; break;
            case 6: obj.interfaces[i].mode = orca::OperationModeSecondRepeater; break;
        }
        
        switch (linkType)
        {
            case 0: obj.interfaces[i].linkType = orca::LinkQualityTypeUnknown; break;
            case 1: obj.interfaces[i].linkType = orca::LinkQualityTypeDbm; break;
            case 2: obj.interfaces[i].linkType = orca::LinkQualityTypeRelative; break;
        }
        
    }
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

} // namespace
