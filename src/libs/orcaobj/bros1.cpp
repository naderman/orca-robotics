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
#include <gbxutilacfr/exceptions.h>
#include <hydroutil/stringutils.h>
#include <gbxutilacfr/mathdefs.h>
#include <orcaice/proputils.h>

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#include "bros1.h"

using namespace std;

namespace orcaobj
{


void
setInit( orca::Frame2d& obj, double x, double y, double orient )
{
    obj.p.x = x;
    obj.p.y = y;
    obj.o   = orient;
}

void
setInit( orca::Frame3d& obj, double x, double y, double z, double pitch, double roll, double yaw )
{
    obj.p.x = x;
    obj.p.y = y;
    obj.p.z = z;
    obj.o.p = pitch;
    obj.o.r = roll;
    obj.o.y = yaw;
}

void
setInit( orca::Twist2d& obj, double vx, double vy, double w )
{
    obj.v.x = vx;
    obj.v.y = vy;
    obj.w   = w;
}

void
setInit( orca::Twist3d& obj, double vx, double vy, double vz, double wx, double wy, double wz )
{
    obj.v.x = vx;
    obj.v.y = vy;
    obj.v.z = vz;
    obj.w.x = wx;
    obj.w.y = wy;
    obj.w.z = wz;
}

void
setInit( orca::Size2d& obj, double l, double w )
{
    obj.l = l;
    obj.w = w;
}

void
setInit( orca::Size3d& obj, double l, double w, double h )
{
    obj.l = l;
    obj.w = w;
    obj.h = h;
}

void
setInit( orca::CartesianPoint2d& obj, double x, double y )
{
    obj.x = x;
    obj.y = y;
}

void
setInit( orca::CartesianPoint& obj, double x, double y, double z )
{
    obj.x = x;
    obj.y = y;
    obj.z = z;
}


std::string toString( const orca::CartesianPoint2d& obj )
{
    std::ostringstream s;
    s << obj.x << " " << obj.y << " x[m], y[m]";
    return s.str();
}

int toCartesianPoint2d( const std::string& s, orca::CartesianPoint2d& obj )
{
    std::istringstream ss( s );
    ss >> obj.x;
    if ( !ss ) return -1;
    ss >> obj.y;
    if ( !ss ) return -1;
    // success
    return 0;
}

std::string toString( const orca::CartesianPoint& obj )
{
    std::ostringstream s;
    s << obj.x << " " << obj.y << " " << obj.z << " x[m], y[m], z[m]";
    return s.str();
}

int toCartesianPoint( const std::string& s, orca::CartesianPoint& obj )
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

std::string toString( const orca::Frame2d& obj )
{
    std::ostringstream s;
//     s << "[" << obj.p.x << "m," << obj.p.y << "m," << obj.o*180.0/M_PI << "deg]";
    s << obj.p.x << " " << obj.p.y << " " << obj.o*180.0/M_PI << " x[m], y[m], yaw[deg]";
    return s.str();
}

int toFrame2d( const std::string& s, orca::Frame2d& obj )
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

std::string toString( const orca::Frame3d& obj )
{
    std::ostringstream s;
//     s << "[" <<obj.p.x<<"m," <<obj.p.y<<"m," <<obj.p.z<<"m, " 
//       << obj.o.r*180.0/M_PI<<"," <<obj.o.p*180.0/M_PI<<"," <<obj.o.y*180.0/M_PI << "deg]";
    s << obj.p.x << " " << obj.p.y << " " << obj.p.z << "   " 
      << obj.o.r*180.0/M_PI << " "<< obj.o.p*180.0/M_PI << " "<< obj.o.y*180.0/M_PI 
      << " x[m], y[m], z[m],   roll[deg], pitch[deg], yaw[deg]";
    return s.str();
}

int toFrame3d( const std::string& s, orca::Frame3d& obj )
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

std::string toString( const orca::Size2d& obj )
{
    std::ostringstream s;
    s << obj.l << " " << obj.w << " length[m], width[m]";
    return s.str();
}

int toSize2d( const std::string& s, orca::Size2d& obj )
{
    std::istringstream ss( s );
    ss >> obj.l;
    if ( !ss ) return -1;
    ss >> obj.w;
    if ( !ss ) return -1;
    // success
    return 0;
}

std::string toString( const orca::Size3d& obj )
{
    std::ostringstream ss;
    ss << obj.l << " " << obj.w << " " << obj.h << " length[m], width[m], height[m]";
    return ss.str();
}

int toSize3d( const std::string& s, orca::Size3d& obj )
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

std::string toString( const orca::Twist2d &t )
{
    std::ostringstream s;
    s << " Twist2d: (v->x,v->y,w): ("
      << t.v.x << ", "
      << t.v.y << ", "
      << t.w*180.0/M_PI << "deg)";
    return s.str();
}

int
getPropertyAsFrame2d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Frame2d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toFrame2d( stringVal, value );
}

orca::Frame2d
getPropertyAsFrame2dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Frame2d & defaultValue )
{
    orca::Frame2d value;
    if ( getPropertyAsFrame2d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsFrame3d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Frame3d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toFrame3d( stringVal, value );
}

orca::Frame3d
getPropertyAsFrame3dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Frame3d & defaultValue )
{
    orca::Frame3d value;
    if ( getPropertyAsFrame3d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsSize2d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Size2d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toSize2d( stringVal, value );
}

orca::Size2d
getPropertyAsSize2dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Size2d & defaultValue )
{
    orca::Size2d value;
    if ( getPropertyAsSize2d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsSize3d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Size3d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toSize3d( stringVal, value );
    return 0;
}

orca::Size3d
getPropertyAsSize3dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Size3d & defaultValue )
{
    orca::Size3d value;
    if ( getPropertyAsSize3d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsCartesianPoint( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::CartesianPoint & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toCartesianPoint( stringVal, value );
    return 0;
}

orca::CartesianPoint
getPropertyAsCartesianPointWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::CartesianPoint & defaultValue )
{
    orca::CartesianPoint value;
    if ( getPropertyAsCartesianPoint( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

// Rotate a 2d point by an angle (about Z-axis)
orca::CartesianPoint2d rotate2d(const orca::CartesianPoint2d &point, const double &angle)
{
    orca::CartesianPoint2d p;
    p.x=point.x*cos(angle)-point.y*sin(angle);
    p.y=point.x*sin(angle)+point.y*cos(angle);

    return p;
}

// Rotate a 3d point by an angle about the X axis
orca::CartesianPoint rotate3dX(const orca::CartesianPoint &point, const double &angle)
{
    orca::CartesianPoint p;
    p.x=point.x;
    p.y=point.y*cos(angle)-point.z*sin(angle);
    p.z=point.y*sin(angle)+point.z*cos(angle);
    return p;
}

// Rotate a 3d point by an angle about the Y axis
orca::CartesianPoint rotate3dY(const orca::CartesianPoint &point, const double &angle)
{
    orca::CartesianPoint p;
    p.x=point.x*cos(angle)+point.z*sin(angle);
    p.y=point.y;
    p.z=-point.x*sin(angle)+point.z*cos(angle);

    return p;
}

// Rotate a 3d point by an angle about the Z axis
orca::CartesianPoint rotate3dZ(const orca::CartesianPoint &point, const double &angle)
{
    orca::CartesianPoint p;
    p.x=point.x*cos(angle)-point.y*sin(angle);
    p.y=point.x*sin(angle)+point.y*cos(angle);
    p.z=point.z;

    return p;
}

// Note: when rotating frames we use negative angles

// Convert a point to another relative to a frame
orca::CartesianPoint2d convertToFrame2d(const orca::Frame2d &frame, const orca::CartesianPoint2d &point)
{
    orca::CartesianPoint2d p;
    // translate
    p.x=point.x-frame.p.x;
    p.y=point.y-frame.p.y;
    // rotate
    p=rotate2d(p,-frame.o);
    return p;
}

// Convert a point to another relative to a frame
orca::CartesianPoint convertToFrame3d(const orca::Frame3d &frame, const orca::CartesianPoint &point)
{
    orca::CartesianPoint p;
    // translate
    p.x=point.x-frame.p.x;
    p.y=point.y-frame.p.y;
    p.z=point.z-frame.p.z;
    // rotate roll pitch yaw
    p=rotate3dX(p,-frame.o.r);
    p=rotate3dY(p,-frame.o.p);
    p=rotate3dZ(p,-frame.o.y);
    return p;
}

// Note the positive angles to revert to the original frame

// Convert a point relative to a frame back to the global frame
orca::CartesianPoint2d convertFromFrame2d(const orca::Frame2d &frame, const orca::CartesianPoint2d &point)
{
    orca::CartesianPoint2d p;
    // first rotate back to global frame
    p=rotate2d(point,frame.o);
    // translate
    p.x+=frame.p.x;
    p.y+=frame.p.y;
    return p;

}

// Convert a point relative to a frame back to the global frame
orca::CartesianPoint convertFromFrame3d(const orca::Frame3d &frame, const orca::CartesianPoint &point)
{
    orca::CartesianPoint p;
    // rotate yaw pitch roll
    p=rotate3dZ(point,frame.o.y);
    p=rotate3dY(p,frame.o.p);
    p=rotate3dX(p,frame.o.r);
    // translate
    p.x+=frame.p.x;
    p.y+=frame.p.y;
    p.z+=frame.p.z;

    return p;
}

orca::Frame3d zeroFrame3d()
{
    orca::Frame3d f;
    f.p.x = 0;
    f.p.y = 0;
    f.p.z = 0;
    f.o.r = 0;
    f.o.p = 0;
    f.o.y = 0;
    return f;
}


} // namespace
