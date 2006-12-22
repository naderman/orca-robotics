/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
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
toLogString( const orca::Waypoint2d &obj )
{
    std::ostringstream s;
    s << obj.target.p.x << " " << obj.target.p.y << " " << obj.target.o*180.0/M_PI << " " 
      << obj.distanceTolerance << " " 
      << obj.headingTolerance*180.0/M_PI << " "
      << toLogString(obj.timeTarget) << " " 
      << obj.maxApproachSpeed << " "
      << obj.maxApproachTurnrate*180.0/M_PI;
    return s.str();
}

std::string 
toLogString( const orca::CpuDataPtr & obj )
{
    std::ostringstream s;
    s << toLogString(obj->timeStamp)
        << " " << obj->userLoad
        << " " << obj->systemLoad
        << " " << obj->idleLoad
        << " " << obj->temperature;
    return s.str();
}

std::string 
toLogString( const orca::LaserScanner2dDataPtr &obj )
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
    s << "\n";

    return s.str();
}

std::string 
toLogString( const orca::Position2dDataPtr &obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj->timeStamp) << " \n";

    // x,y position, orientation(deg), and respective velocities
    s << obj->pose.p.x << " " 
      << obj->pose.p.y << " "
      << RAD2DEG(obj->pose.o) << " "
      << obj->motion.v.x << " "
      << obj->motion.v.y << " "
      << RAD2DEG(obj->motion.w) << "\n";

    return s.str();
}

std::string 
toLogString( const orca::Localise2dDataPtr &obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj->timeStamp) << endl;

    // number of hypotheses on a line
    s << obj->hypotheses.size() << endl;

    for ( unsigned int i=0; i < obj->hypotheses.size(); i++ )
    {
        const orca::Pose2dHypothesis &h = obj->hypotheses[i];

        s << h.mean.p.x << " "
          << h.mean.p.y << " "
          << RAD2DEG(h.mean.o) << " "
          << h.cov.xx << " "
          << h.cov.xy << " "
          << h.cov.yy << " "
          << h.cov.xt << " "
          << h.cov.yt << " "
          << h.cov.tt << " "
          << h.weight << endl;
    }
    return s.str();
}

std::string 
toLogString( const orca::Position3dDataPtr &obj )
{
    std::stringstream s;
    
    // timestamp on the first line
    s << toLogString(obj->timeStamp) << " \n";

    // x,y,z position, roll,pitch,yaw(deg), and respective velocities on the second line
    s << obj->pose.p.x << " " 
      << obj->pose.p.y << " "
      << obj->pose.p.z << " "
      << RAD2DEG(obj->pose.o.r) << " "
      << RAD2DEG(obj->pose.o.p) << " "
      << RAD2DEG(obj->pose.o.y) << " \n"
      << obj->motion.v.x << " "
      << obj->motion.v.y << " "
      << obj->motion.v.z << " "
      << RAD2DEG(obj->motion.w.x) << " "
      << RAD2DEG(obj->motion.w.y) << " "
      << RAD2DEG(obj->motion.w.z) << " \n";

    return s.str();
}

// std::string 
// toLogString( const orca::PolarFeature2dDataPtr & obj )
// {
//     std::stringstream s;

//     // timestamp on the first line
//     s << toLogString(obj->timeStamp) << " \n";

//     // feature type, range, and bearing (deg) on second line
//     const orca::PolarFeature2dSequence &features = obj->features;
//     for (unsigned int i=0; i < obj->features.size(); i++)
//     {
//         if ( features[i] == 0 )
//         {
//             s << "  " << i << ": ERROR: Feature was NULL!" << endl;
//         }
//         else
//         {
//             const orca::SinglePolarFeature2dPtr &f = features[i];
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
toLogString( const orca::PowerData &obj )
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
fromLogString( std::stringstream & stream, orca::PowerData & obj )
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
toLogString( const orca::PathPlanner2dTaskPtr &obj )
{
    std::stringstream s;

    // first line: timestamp, number of waypoints
    s << toLogString(obj->timeStamp) << " " 
      << obj->coarsePath.size() << " \n";
    
    for ( unsigned int i=0; i < obj->coarsePath.size(); i++ )
    {
        s << toLogString(obj->coarsePath[i]) << " ";
    }
    s << "\n";

    return s.str();
}

std::string 
toLogString( const orca::PathPlanner2dDataPtr &obj )
{
    std::stringstream s;
    
    // first line: timestamp, result code, number of waypoints
    s << toLogString(obj->timeStamp) << " "
      << obj->result << " "
      << obj->path.size() << "\n";
    
    // second line: path
    for ( unsigned int i=0; i < obj->path.size(); i++ )
    {
        s << toLogString(obj->path[i]) << " ";
    }
    s << "\n";

    return s.str();
}


std::string 
toLogString( const orca::Time & obj )
{
    std::ostringstream s;
    s << obj.seconds << " " << obj.useconds;
    return s.str();
}
void 
fromLogString( std::stringstream & stream, orca::Time & obj )
{
    stream >> obj.seconds >> obj.useconds;
}

} // namespace
