/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_PATH_UTILS_H
#define ORCA_PATH_UTILS_H

#include <QPointF>
#include <QVector>
#include <orca/pathfollower2d.h>
#include <orcaice/orcaice.h>

namespace orcaqgui2d {

const int QT_ANGLE_MULTIPLIER = 16;    
    
//
// Internal Gui representation for waypoint and path
// Author: Tobias Kaupp 
//
class GuiWaypoint
{
public:
    GuiWaypoint()
    : position(QPointF(0.0,0.0)), 
      heading(0), 
      timeTarget(0.0), 
      distanceTolerance(0.0), 
      headingTolerance(0.0), 
      maxSpeed(0.0),
      maxTurnrate(0)
    {}
    
    QPointF position;           // in m
    int     heading;            // in 1/16 deg from 0 to 360*16
    float   timeTarget;         // number of seconds until arrival at waypoint
    float   distanceTolerance;  // distance tolerances in m
    int     headingTolerance;   // heading tolerances in 1/16 deg from 0 to 360*16
    float   maxSpeed;           // max speed in m/s
    int     maxTurnrate;        // max turnrate in deg/s
};

typedef QVector<GuiWaypoint> GuiPath;

//
// Tolerances and other parameters of waypoint
// The units are the same as above
//
class WaypointSettings
{
    public:
        WaypointSettings() {};
        WaypointSettings(std::string a, float b, float c, int d, float e, int f):
            spacingProperty(a), 
            spacingValue(b), 
            distanceTolerance(c), 
            headingTolerance(d), 
            maxApproachSpeed(e), 
            maxApproachTurnrate(f)
            {}
        std::string spacingProperty;
        float spacingValue;
        float distanceTolerance;
        int headingTolerance;
        float maxApproachSpeed;
        int maxApproachTurnrate;
};

// Compute straight line distance of two points in [m]   
float straightLineDist( QPointF line );

//
// Conversion functions
//
void guiPathToOrcaPath( const GuiPath &in, 
                        orca::Path2d  &out, 
                        int            numLoops = 1, 
                        float          timeOffset = 0.0 );

void orcaPathToGuiPath( const orca::Path2d &in, 
                        GuiPath            &out );

//
// Reading stuff from cfg file
//
WaypointSettings readWaypointSettings( const Ice::PropertiesPtr &props, 
                                       const std::string        &tag );

bool readActivateImmediately( const Ice::PropertiesPtr &props, 
                              const std::string        &tag );

QString readDumpPath( const Ice::PropertiesPtr &props, 
                      const std::string        &tag );


}

#endif
