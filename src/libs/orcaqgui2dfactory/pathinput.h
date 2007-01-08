/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include <QTableWidget>

namespace orcaqgui {

class OrcaGuiUserEvent;
class IHumanManager;
class PathInput;
class WpTable;

// all units are SI units
class WaypointSettings
{
    public:
        WaypointSettings(std::string a, float b, float c, int d, float e, int f, int g):
            spacingProperty(a), spacingValue(b), distanceTolerance(c), headingTolerance(d), maxApproachSpeed(e), maxApproachTurnrate(f), numberOfLoops(g)
            {}
            std::string spacingProperty;
            float spacingValue;
            float distanceTolerance;
            int headingTolerance;
            float maxApproachSpeed;
            int maxApproachTurnrate;
            int numberOfLoops;
};

class WpWidget : public QWidget
{
    Q_OBJECT
    
    public:
        WpWidget(   PathInput *pathInput,
                    QPolygonF *waypoints, 
                    QVector<int> *headings,
                    QVector<float> *times,
                    QVector<float> *waitingTimes,
                    QVector<float> *distTolerances,
                    QVector<int> *headingTolerances,
                    QVector<float> *maxSpeeds,
                    QVector<int> *maxTurnrates);
        ~WpWidget() {};
        void refreshTable();
    
    private:
        PathInput *pathInput_;
        WpTable *wpTable_;
        bool pathFileSet_;
        QString pathFileName_;
    
    private slots:
        void savePathAs();
        void savePath();
        void loadPath();
    
};

class WpTable : public QTableWidget
{
    Q_OBJECT
            
    public:
        WpTable( QWidget *parent,
                 PathInput *pathInput,
                 QPolygonF *waypoints, 
                 QVector<int> *headings,
                 QVector<float> *times,
                 QVector<float> *waitingTimes,
                 QVector<float> *distTolerances,
                 QVector<int> *headingTolerances,
                 QVector<float> *maxSpeeds,
                 QVector<int> *maxTurnrates );
        ~WpTable() {};
        void refreshTable();
        
    private:
        PathInput *pathInput_;
        
        // data which is shared with pathinput
        QPolygonF *waypoints_;
        QVector<int> *headings_;
        QVector<float> *times_;
        QVector<float> *waitingTimes_;
        QVector<float> *distTolerances_;
        QVector<int> *headingTolerances_;
        QVector<float> *maxSpeeds_;
        QVector<int> *maxTurnrates_;
        
        // this one is only local
        QVector<float> velocities_;
        
        void computeVelocities();
        
        // lock up the cellUpdate signal: it should only be emitted if the user changes cell entries
        // not if we programmatically change them
        bool isLocked_;
    
    private slots:
        void updateDataStorage(int row, int column);
};

class PathInput : public QObject
{ 
    Q_OBJECT
            
    public:
        PathInput( WaypointSettings *wpSettings );
        virtual ~PathInput();  
     
        virtual void paint( QPainter *painter );
        virtual void setTransparency( bool useTransparency ) { useTransparency_=useTransparency; };
        
        virtual void processPressEvent( QMouseEvent* e);
        virtual void processReleaseEvent( QMouseEvent* e );
        virtual void processDoubleClickEvent( QMouseEvent* e);
        virtual void processMoveEvent( QMouseEvent* e);
        
        virtual void updateWpSettings( WaypointSettings* wpSettings );
        
        virtual void savePath( const QString &fileName, IHumanManager *humanManager = 0 ) const;
        virtual void loadPath( QString* fileName ); 
        
    protected:    
        WaypointSettings *wpSettings_;
        QMatrix wmInv_; // win2mm matrix
        
        bool useTransparency_;
        
        void addWaypoint( QPointF );
        void removeWaypoint( QPointF ); 
        void changeWpParameters( QPointF );
        void resizeData( int index );
                
        // This is the data for drawing. It gets translated into the relevant Ice data structures.
        // The WpWidget has full read/write access to it by passing a pointer.
        QPolygonF      waypoints_;           // in m
        QVector<int>   headings_;            // in 1/16 deg from 0 to 360*16
        QVector<float> times_;               // number of seconds until arrival at waypoint
        QVector<float> distTolerances_;      // distance tolerances in m
        QVector<int>   headingTolerances_;   // heading tolerances in 1/16 deg from 0 to 360*16
        QVector<float> maxSpeeds_;           // max speed in m/s
        QVector<int> maxTurnrates_;          // max turnrate in deg/s
        QVector<float> waitingTimes_;        // how long to wait at this waypoint

        QPointF mouseDownPnt_;
        QPointF mouseUpPnt_;
        QPointF mouseMovePnt_;
        QPointF doubleClick_;
        
        WpWidget *wpWidget_;
        int waypointInFocus_;
        
    public slots:
        void setWaypointFocus(int row, int column);
        void generateFullPath();
        
};

class PathFollowerInput : public PathInput
{ 
    public:
        PathFollowerInput( WaypointSettings *wpSettings );
        virtual ~PathFollowerInput() {};  

        orca::PathFollower2dData getPath() const;
};

class PathPlannerInput : public PathInput
{ 
public:
    PathPlannerInput( WaypointSettings *wpSettings )
        :PathInput( wpSettings )
        {};
    
    virtual ~PathPlannerInput() {};  
    
    orca::PathPlanner2dTask getTask() const;
};

WaypointSettings readWaypointSettings( const Ice::PropertiesPtr & props, const std::string & tag );

bool readActivateImmediately( const Ice::PropertiesPtr & props, const std::string & tag );

} // namespace

#endif
