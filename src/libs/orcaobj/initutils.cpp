/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <sstream>
#include <cmath>
#include <IceUtil/Time.h>

#include <orcaobj/initutils.h>
#include <orcaobj/timeutils.h>

// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Random doubles and integers
#define RDOUBLE (rand()*M_PI)
#define RINT    (rand())
#define RCHAR   ((char)rand())

using namespace std;

namespace orcaice
{

void
setInit( orca::Frame2d & obj )
{
    obj.p.x = 0.0;
    obj.p.y = 0.0;
    obj.o   = 0.0;
}

void
setInit( orca::Frame3d & obj )
{
    obj.p.x = 0.0;
    obj.p.y = 0.0;
    obj.p.z = 0.0;
    obj.o.p = 0.0;
    obj.o.r = 0.0;
    obj.o.y = 0.0;
}

void
setInit( orca::Size3d & obj )
{
    obj.l = 0.0;
    obj.w = 0.0;
    obj.h = 0.0;
}

void
setInit( orca::CartesianPoint & obj )
{
    obj.x = 0.0;
    obj.y = 0.0;
    obj.z = 0.0;
}

void
setInit( orca::Time & obj )
{
    obj.seconds = 0;
    obj.useconds = 0;
}

void
setInit( const orca::Position2dDataPtr & obj )
{
    orca::Time t;
    setInit( t );
    obj->timeStamp = t;

    obj->pose.p.x = 0.0;
    obj->pose.p.y = 0.0;
    obj->pose.o = 0.0;
    obj->motion.v.x = 0.0;
    obj->motion.v.y = 0.0;
    obj->motion.w = 0.0;
    obj->stalled=false;
}

void 
setInit( orca::Waypoint2d & wp )
{
    setInit(wp.target);
    wp.distanceTolerance = 0.0;
    wp.headingTolerance = 0.0;
    setInit(wp.timeTarget);
    wp.maxApproachSpeed = 0.0;
    wp.maxApproachTurnrate = 0.0;
}

void 
setSane( orca::Time & obj )
{
    obj = toOrcaTime( IceUtil::Time::now() );
}

void 
setSane( orca::TimeOfDay & obj )
{
    obj = toOrcaTimeOfDay( IceUtil::Time::now() );

//     obj.hours   = RINT%24;
//     obj.minutes = RINT%59;
//     obj.seconds = double(RINT%59000)/1000.0;
}

void 
setSane( orca::Date & obj )
{
    obj = toOrcaDate( IceUtil::Time::now() );

//     obj.day   = RINT%31;
//     obj.month = RINT%12;
//     obj.year  = RINT%3000;
}

void 
setSane( const orca::Position2dDataPtr & obj )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->pose.p.x = double(RINT%100000)/1000.0;
    obj->pose.p.y = double(RINT%100000)/1000.0;
    obj->pose.o = double(RINT%100000)/1000.0;
    obj->motion.v.x = double(RINT%100000)/1000.0;
    obj->motion.v.y = double(RINT%100000)/1000.0;
    obj->motion.w = double(RINT%100000)/1000.0;
    obj->stalled=false;
}

void 
setSane( const orca::Position3dDataPtr & obj )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->pose.p.x = double(RINT%100000)/1000.0;
    obj->pose.p.y = double(RINT%100000)/1000.0;
    obj->pose.p.z = double(RINT%100000)/1000.0;
    obj->pose.o.p = double(RINT%100000)/1000.0;
    obj->pose.o.r = double(RINT%100000)/1000.0;
    obj->pose.o.y = double(RINT%100000)/1000.0;
    obj->motion.v.x = double(RINT%100000)/1000.0;
    obj->motion.v.y = double(RINT%100000)/1000.0;
    obj->motion.v.z = double(RINT%100000)/1000.0;
    obj->motion.w.x = double(RINT%100000)/1000.0;
    obj->motion.w.y = double(RINT%100000)/1000.0;
    obj->motion.w.z = double(RINT%100000)/1000.0;
}

void 
setSane( const orca::GpsDataPtr & obj )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    setSane( obj->utcTime );

    obj->latitude = double(RINT%90000)/1000.0;
    obj->longitude = double(RINT%360000)/1000.0;
    obj->altitude = double(RINT%90000)/1000.0;

    obj->speed = double(RINT%1000000)/1000.0;
    obj->climbRate = double(RINT%1000000)/1000.0;

    obj->satellites = RINT%50;
    obj->positionType = RINT%2;

    obj->geoidalSeparation = double(RINT%10000000)/1000.0;
}


void 
setSane( const orca::GpsTimeDataPtr & obj )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    setSane( obj->utcTime );
    setSane( obj->utcDate );
}


void 
setSane( const orca::GpsMapGridDataPtr & obj )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    setSane( obj->utcTime );

    obj->zone = RINT%500;

    obj->northing = double(RINT%800000)/1000.0;
    obj->easting = double(RINT%800000)/1000.0;
    obj->altitude = double(RINT%90000)/1000.0;

    obj->speed = double(RINT%1000000)/1000.0;
    obj->climbRate = double(RINT%1000000)/1000.0;

    obj->positionType = RINT%2;
}

void 
setSane( const orca::PowerDataPtr & obj, int count )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    // set up data structure for 3 batteries
    orca::BatteryData bd;
    for ( int i=0; i<count; ++i ) 
    {   
        stringstream ss;
        ss << "battery" << i;
        bd.name = ss.str();
        bd.voltage = float(double(RINT%100000)/1000.0);
        bd.percent = float(double(RINT%100000)/1000.0);
        bd.secRemaining = float(double(RINT%1000000)/1000.0);

        obj->batteries.push_back( bd );
    }
}

void 
setSane( const orca::Velocity2dCommandPtr & obj )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->motion.v.x = double(RINT%100000)/1000.0;
    obj->motion.v.y = double(RINT%100000)/1000.0;
    obj->motion.w = double(RINT%100000)/1000.0;
}

void 
setSane( const orca::LaserScanner2dDataPtr & obj, int numberOfSamples )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->maxRange = 80.0;
    obj->fieldOfView = M_PI;
    obj->startAngle = -(obj->fieldOfView/2.0);

    for ( int i=0; i<numberOfSamples; ++i ) {
        obj->ranges.push_back( fmod((float)RINT,80) );
        obj->intensities.push_back( char(RINT%8) );
    }
}

void 
setSane( const orca::OgMapDataPtr & obj, int width, int height )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->numCellsX = width;
    obj->numCellsY = height;
    obj->origin.p.x = 0;
    obj->origin.p.y = 0;
    obj->origin.o   = 0;
    obj->metresPerCellX = 1.0;
    obj->metresPerCellY = 1.0;
    obj->data.resize( 640*480, char(88) );
}

void 
setSane( const orca::CameraDataPtr & obj, int width, int height )
{
    orca::Time t;
    setSane( t );
    obj->timeStamp = t;

    obj->imageWidth = width;
    obj->imageHeight = height;
    obj->format = orca::ImageFormatModeNfi;
    obj->compression = orca::ImageCompressionNone;
    // assume RGB/BGR mode
    obj->image.resize( obj->imageWidth*obj->imageHeight*3, char(88) );
}

} // namespace
