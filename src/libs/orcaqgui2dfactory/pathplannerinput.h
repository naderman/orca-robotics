/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER_INPUT_H
#define PATHPLANNER_INPUT_H

#include <orca/pathplanner2d.h>
#include <orcaqgui2dfactory/pathutils.h>
#include <orcaqgui2dfactory/ipathinput.h>
#include <orcaqgui2dfactory/pathdesignscreen.h>
#include "pathdesigntablewidget.h"
#include "pathfilehandler.h"

namespace orcaqgui2d {
    
class PathPlannerHI;

class PathPlannerInput : public IPathInput
{ 
public:
    PathPlannerInput( PathPlannerHI                       &pathPlannerHI,
                      WaypointSettings                    *wpSettings,
                      hydroqguielementutil::IHumanManager &humanManager );
    
    virtual ~PathPlannerInput() {};  
    
    virtual void paint( QPainter *painter );
    virtual void setUseTransparency( bool useTransparency );
        
    virtual void processPressEvent( QMouseEvent* e);
    virtual void processReleaseEvent( QMouseEvent* e );
    virtual void processDoubleClickEvent( QMouseEvent* e) {};
    virtual void processMoveEvent( QMouseEvent* e) {};
    virtual void updateWpSettings( WaypointSettings* wpSettings );
        
    virtual void savePath( const QString &filename );
    virtual void loadPath( const QString &filename );
    virtual void loadPreviousPath();
    virtual void sendPath();
    virtual void cancelPath();
    
    virtual void setWaypointFocus( int waypointId );
    
    orca::PathPlanner2dTask getTask() const;
            
    private:
        PathPlannerHI &pathPlannerHI_;
        
        std::auto_ptr<PathDesignScreen> pathDesignScreen_;
        std::auto_ptr<PathDesignWidget> pathDesignWidget_;
        std::auto_ptr<GuiPath> guiPath_;
        std::auto_ptr<PathFileHandler> pathFileHandler_;

};

}

#endif
