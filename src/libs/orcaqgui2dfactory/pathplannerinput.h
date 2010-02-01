/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER_INPUT_H
#define PATHPLANNER_INPUT_H

#include <orca/pathplanner2d.h>
#include <hydroqguipath/pathdesignscreen.h>
#include <hydroqguipath/pathdesigntablewidget.h>
#include <hydroqguipath/ipathinput.h>
#include <hydroqguipath/pathutils.h>
#include <orcaqgui2dfactory/pathfilehandler.h>

namespace orcaqgui2d {
    
class PathPlannerUserInteraction;

//!
//! An implementation of PathInput for the PathPlanner
//! Enables user to click on screen and manipulate path using a table
//!
//! @author Tobias Kaupp
//!
class PathPlannerInput : public hydroqguipath::IPathInput
{ 
public:
    PathPlannerInput( PathPlannerUserInteraction          &pathPlannerUI,
                      hydroqguipath::WaypointSettings     *wpSettings,
                      hydroqguielementutil::IHumanManager &humanManager );
    
    virtual ~PathPlannerInput() {};  
    
    virtual void paint( QPainter *painter );
    virtual void setUseTransparency( bool useTransparency );
        
    virtual void processPressEvent( QMouseEvent* e);
    virtual void processReleaseEvent( QMouseEvent* e );
    virtual void processDoubleClickEvent( QMouseEvent* e) {};
    virtual void processMoveEvent( QMouseEvent* e) {};
    virtual void updateWpSettings( hydroqguipath::WaypointSettings* wpSettings );
        
    virtual void savePath();
    virtual void loadPath();
    virtual void loadPreviousPath();
    virtual void sendPath();
    virtual void cancelPath();
    
    virtual void setWaypointFocus( int waypointId );
    virtual void getPath( hydroqguipath::GuiPath &guiPath, int &numLoops, float &timeOffset ) const {};
    virtual void switchTableView( bool onOff ) {};
    
    orca::PathPlanner2dTask getTask() const;
    
            
private:
        
    PathPlannerUserInteraction &pathPlannerUI_;
    
    std::auto_ptr<hydroqguipath::PathDesignScreen> pathDesignScreen_;
    std::auto_ptr<hydroqguipath::PathDesignTableWidget> pathDesignTableWidget_;
    std::auto_ptr<hydroqguipath::GuiPath> guiPath_;

};

}

#endif
