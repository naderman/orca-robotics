/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>

#include "stringutils.h"

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

// this is a copy from orcaice/mathdefs.h
// make a copy here to avoid dependency
//==============
#ifndef DEG2RAD_RATIO
#   define DEG2RAD_RATIO	(M_PI/180.0)
#endif

// Converts from degrees to radians.
#define DEG2RAD(deg)	((deg)*DEG2RAD_RATIO)
// Converts from radians to degrees.
#define RAD2DEG(rad) 	((rad)/DEG2RAD_RATIO)
//==============

using namespace std;

namespace hydrobros1
{

std::string 
toString( const hydrobros1::CartesianPoint2d& obj )
{
    std::ostringstream s;
    s << obj.x << " " << obj.y << " x[m], y[m]";
    return s.str();
}

int 
toCartesianPoint2d( const std::string& s, hydrobros1::CartesianPoint2d& obj )
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
toString( const hydrobros1::CartesianPoint3d& obj )
{
    std::ostringstream s;
    s << obj.x << " " << obj.y << " " << obj.z << " x[m], y[m], z[m]";
    return s.str();
}

int 
toCartesianPoint( const std::string& s, hydrobros1::CartesianPoint3d& obj )
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
toString( const hydrobros1::Frame2d& obj )
{
    std::ostringstream s;
//     s << "[" << obj.p.x << "m," << obj.p.y << "m," << obj.o*180.0/M_PI << "deg]";
    s << obj.p.x << " " << obj.p.y << " " << obj.o*180.0/M_PI << " x[m], y[m], yaw[deg]";
    return s.str();
}

int 
toFrame2d( const std::string& s, hydrobros1::Frame2d& obj )
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
toString( const hydrobros1::Frame3d& obj )
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
toFrame3d( const std::string& s, hydrobros1::Frame3d& obj )
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
toString( const hydrobros1::Size2d& obj )
{
    std::ostringstream s;
    s << obj.l << " " << obj.w << " length[m], width[m]";
    return s.str();
}

int 
toSize2d( const std::string& s, hydrobros1::Size2d& obj )
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
toString( const hydrobros1::Size3d& obj )
{
    std::ostringstream ss;
    ss << obj.l << " " << obj.w << " " << obj.h << " length[m], width[m], height[m]";
    return ss.str();
}

int 
toSize3d( const std::string& s, hydrobros1::Size3d& obj )
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

std::string
toString( const hydrobros1::Twist2d &t )
{
    std::ostringstream s;
    s << " Twist2d: (v->x,v->y,w): ("
      << t.v.x << ", "
      << t.v.y << ", "
      << t.w*180.0/M_PI << "deg)";
    return s.str();
}

} // namespace
