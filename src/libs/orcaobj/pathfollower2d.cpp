/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <cmath>
#include "pathfollower2d.h"
#include "datetime.h"
#include "bros1.h"

using namespace std;

namespace orcaobj
{

bool isSane( const orca::PathFollower2dData& pathData, std::string& reason )
{
    std::stringstream ss;
    bool sane=true;
    for ( unsigned int i=0; i < pathData.path.size(); i++ )
    {
        const orca::Waypoint2d &wp = pathData.path[i];

        if ( wp.distanceTolerance < 0.0 )
        {
            ss << "Waypoint " << i << ": bad distance tolerance: " 
               << wp.distanceTolerance << "m" << endl;
            sane = false;
        }
        if ( wp.headingTolerance < 0.0 )
        {
            ss << "Waypoint " << i << ": bad heading tolerance: " 
               << wp.headingTolerance*180.0/M_PI << "deg" << endl;
            sane = false;
        }
        if ( wp.maxApproachSpeed < 0.0 )
        {
            ss << "Waypoint " << i << ": bad maxApproachSpeed: " 
               << wp.maxApproachSpeed << "m/s" << endl;
            sane = false;
        }
        if ( wp.maxApproachTurnrate < 0.0 )
        {
            ss << "Waypoint " << i << ": bad maxApproachTurnrate: " 
               << wp.maxApproachTurnrate*180.0/M_PI << "deg/s" << endl;
            sane = false;
        }

    }
    reason = ss.str();
    return sane;
}


bool
isPathSketchy( const orca::Path2d& path, std::string &sketchyReason )
{
    std::stringstream ss;
    bool normal=true;
    const float epsLinear     = 1e-3;
    const float epsRotational = 1.0*M_PI/180.0;
    for ( unsigned int i=0; i < path.size(); i++ )
    {
        const orca::Waypoint2d &wp = path[i];

        if ( wp.distanceTolerance < epsLinear )
        {
            ss << "Waypoint " << i << ": possibly sketchy distance tolerance: " 
               << wp.distanceTolerance << "m" << endl;
            normal = false;
        }
        if ( wp.headingTolerance < epsRotational )
        {
            ss << "Waypoint " << i << ": possibly sketchy heading tolerance: " 
               << wp.headingTolerance*180.0/M_PI << "deg" << endl;
            normal = false;
        }
        if ( wp.maxApproachSpeed < epsLinear )
        {
            ss << "Waypoint " << i << ": possibly sketchy maxApproachSpeed: " 
               << wp.maxApproachSpeed << "m/s" << endl;
            normal = false;
        }
        if ( wp.maxApproachTurnrate < epsRotational )
        {
            ss << "Waypoint " << i << ": possibly sketchy maxApproachTurnrate: " 
               << wp.maxApproachTurnrate*180.0/M_PI << "deg/s" << endl;
            normal = false;
        }
        if ( wp.timeTarget.seconds < 0 || wp.timeTarget.useconds < 0 )
        {
            ss << "Waypoint " << i << ": funky timeTarget: "
               << wp.timeTarget.seconds << ":" << wp.timeTarget.useconds << endl;
            normal = false;
        }
    }
    if ( !normal )
    {
        sketchyReason = ss.str();
        return true;
    }
    else
        return false;
}


std::string toString( const orca::PathFollower2dData& obj )
{
    std::ostringstream s;
    s << toString(obj.timeStamp)
      << " PathFollower2dData: " << obj.path.size() << " waypoints.";
    return s.str();
}

std::string toString( const orca::Waypoint2d &obj )
{
    std::ostringstream s;
    s << "target=" << toString(obj.target) 
      << ", tolerances=[" << obj.distanceTolerance << "," << obj.headingTolerance*180.0/M_PI << "deg]"
      << ", timeTarget=[" << obj.timeTarget.seconds << ":" << obj.timeTarget.useconds << "]"
      << ", maxApproach=[" << obj.maxApproachSpeed << "m/s," << obj.maxApproachTurnrate*180.0/M_PI << "deg/s]";
    return s.str();
}

std::string toVerboseString( const orca::PathFollower2dData& obj )
{
    std::stringstream s;

    s << orcaobj::toString(obj.timeStamp)
      << " PathFollower2dData [" << obj.path.size() << " waypoints]:" << endl;
    for ( unsigned int i=0; i < obj.path.size(); i++ )
    {
        s << "  " << i << ":" << orcaobj::toString(obj.path[i]) << endl;
    }

    return s.str();
}

} // namespace
