/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATH_INPUT_H
#define ORCA2_PATH_INPUT_H

#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>
#include <orcaice/context.h>
#include <QMatrix>
#include <QVector>
#include <QMouseEvent>

namespace orcaqgui {

class OrcaGuiUserEvent;
class IHumanManager;

// all units are SI units
class WaypointSettings
{
    public:
        WaypointSettings(float a, float b, int c, float d, int e):
            timePeriod(a), distanceTolerance(b), headingTolerance(c), maxApproachSpeed(d), maxApproachTurnrate(e)
            {}
            float timePeriod;
            float distanceTolerance;
            int headingTolerance;
            float maxApproachSpeed;
            int maxApproachTurnrate;
};

class PathInput
{ 
    public:
        PathInput( WaypointSettings *wpSettings );
        virtual ~PathInput() {};  
     
        virtual void paint( QPainter *painter );
        virtual void setTransparency( bool useTransparency ) { useTransparency_=useTransparency; };
        
        virtual void processPressEvent( QMouseEvent* e);
        virtual void processReleaseEvent( QMouseEvent* e );
        virtual void processDoubleClickEvent( QMouseEvent* e);
        virtual void processMoveEvent( QMouseEvent* e);
        
        virtual void updateWpSettings( WaypointSettings* wpSettings );
        
        virtual void savePath( const QString &fileName, IHumanManager *humanManager ) const;
        

    protected:    
        WaypointSettings *wpSettings_;
        QMatrix wmInv_; // win2mm matrix
        
        bool useTransparency_;
        
        void addWaypoint( QPointF );
        void removeWaypoint( QPointF ); 
        void changeWpParameters( QPointF );
        void resizeData( int index );
                
        QPolygonF      waypoints_;           // in m
        QVector<int>   headings_;            // in 1/16 deg from 0 to 360*16
        QVector<float> times_;               // number of seconds until arrival at waypoint
        QVector<float> distTolerances_;      // distance tolerances in m
        QVector<int>   headingTolerances_;   // heading tolerances in 1/16 deg from 0 to 360*16
        
        QVector<float> maxSpeeds_;           // max speed in m/s
        QVector<int> maxTurnrates_;          // max turnrate in deg/s

        QPointF mouseDownPnt_;
        QPointF mouseUpPnt_;
        QPointF mouseMovePnt_;
        QPointF doubleClick_;
};

class PathFollowerInput : public PathInput
{ 
    public:
        PathFollowerInput( WaypointSettings *wpSettings );
        virtual ~PathFollowerInput() {};  

        orca::PathFollower2dDataPtr getPath() const;
       
        // loading the path is done from here, saving is done by the pathpainter
        virtual void loadDataFromFile( QString* fileName );   
        
};

class PathPlannerInput : public PathInput
{ 
public:
    PathPlannerInput( WaypointSettings *wpSettings )
        :PathInput( wpSettings )
        {};
    
    virtual ~PathPlannerInput() {};  
    
    orca::PathPlanner2dTaskPtr getTask() const;
};

WaypointSettings
readWaypointSettings( const orcaice::Context  &context );
bool readActivateImmediately( orcaice::Context &context );

}

#endif
