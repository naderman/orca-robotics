/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_PATH_INPUT_H
#define ORCA_PATH_INPUT_H

#include <QMatrix>
#include <QVector>
#include <QTableWidget>
#include <QSpinBox>

#include <orcaice/orcaice.h>
#include <orca/pathplanner2d.h>
#include "ipathinput.h"
#include "pathutils.h"

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace orcaqgui2d {

class PathInput;
class WpTable;


class WpWidget : public QWidget
{
    Q_OBJECT
    
    public:
        WpWidget(   PathInput *pathInput,
                    GuiPath *guiPath,
                    QVector<float> *waitingTimes );
        ~WpWidget() {};
        void refreshTable();
        QString getBehaviour( int row );
        int numberOfLoops() { return numLoopsSpin_->value(); };
    
    private:
        PathInput *pathInput_;
        WpTable *wpTable_;
        bool pathFileSet_;
        QString pathFileName_;
        QSpinBox *numLoopsSpin_;
    
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
                 GuiPath *guiPath,
                 QVector<float> *waitingTimes );
        ~WpTable() {};
        void refreshTable();
        void computeVelocities();
        QString getBehaviour( int row );
        
    private:
        PathInput *pathInput_;
        
        // data which is shared with pathinput
        GuiPath *guiPath_;
        QVector<float> *waitingTimes_;
        
        // this one is only local
        QVector<float> velocities_;
        
        // lock up the cellUpdate signal: it should only be emitted if the user changes cell entries
        // not if we programmatically change them (as in refreshTable)
        bool isLocked_;
    
    private slots:
        // called after user interaction
        void updateDataStorage(int row, int column);
};

class PathInput : public QObject, public IPathInput
{ 
    Q_OBJECT
            
    public:
        PathInput( QObject *parent,
                   WaypointSettings *wpSettings,
                   hydroqguielementutil::IHumanManager &humanManager,
                   QString lastSavedPathFile="" );
        virtual ~PathInput();  
     
        virtual void paint( QPainter *painter );
        virtual void setUseTransparency( bool useTransparency ) { useTransparency_=useTransparency; };
        
        virtual void processPressEvent( QMouseEvent* e);
        virtual void processReleaseEvent( QMouseEvent* e );
        virtual void processDoubleClickEvent( QMouseEvent* e);
        virtual void processMoveEvent( QMouseEvent* e);
        
        virtual void updateWpSettings( WaypointSettings* wpSettings );
        
        virtual void savePath( const QString &fileName );
        virtual void loadPath( const QString &fileName ); 
        
    protected:    
        WaypointSettings *wpSettings_;
        hydroqguielementutil::IHumanManager &humanManager_;
        QMatrix wmInv_; // win2mm matrix
        
        bool useTransparency_;
        
        void addWaypoint( QPointF );
        void removeWaypoint( QPointF ); 
        void changeWpParameters( QPointF );
        void resizeData( int index );
                
        // The path in Gui representation. Shared with wpWidget
        GuiPath guiPath_;
        // how long to wait at this waypoint, filled in by wpWidget
        QVector<float> waitingTimes_;        

        QPointF mouseDownPnt_;
        QPointF mouseUpPnt_;
        QPointF mouseMovePnt_;
        QPointF doubleClick_;
        
        WpWidget *wpWidget_;
        int waypointInFocus_;
        
        QString lastSavedPathFile_;
        
        float secondsToCompleteLoop() const;
        
    public slots:
        void setWaypointFocus(int row, int column);
        void generateFullPath();
        void loadPreviousPath();
        
    signals:
        void sendPathClicked();
        void cancelPathClicked();
        
    private:
        void expandPath( int index, int numInsert, int headingTolerance);
        int  expandPathStationary( int index );
        int  expandPathLeftRight( int index );
        int  expandPathRightLeft( int index );
        int  expandPathTurn360( int index );
        
};

class PathFollowerInput : public PathInput
{ 
    public:
        PathFollowerInput( QObject                  *parent,
                           WaypointSettings         *wpSettings,
                           hydroqguielementutil::IHumanManager &humanManager,
                           QString                   lastSavedPathFile )
            : PathInput( parent, wpSettings, humanManager, lastSavedPathFile )
        {};
        virtual ~PathFollowerInput() {};  

        bool getPath( orca::PathFollower2dData &pathData ) const;
};

class PathPlannerInput : public PathInput
{ 
public:
    PathPlannerInput( QObject                  *parent,
                      WaypointSettings         *wpSettings,
                      hydroqguielementutil::IHumanManager &humanManager )
        :PathInput( parent, wpSettings, humanManager )
        {};
    
    virtual ~PathPlannerInput() {};  
    
    orca::PathPlanner2dTask getTask() const;
};

} // namespace

#endif
