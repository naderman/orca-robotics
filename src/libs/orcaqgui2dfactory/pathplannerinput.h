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

#include <QObject>
#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaqgui2dfactory/pathutils.h>
#include <orcaqgui2dfactory/pathdesignscreen.h>
#include <orca/pathplanner2d.h>
#include "ipathinput.h"

namespace orcaqgui2d {

class PathPlannerInput : public IPathInput
{ 
public:
    PathPlannerInput( QObject                  *parent,
                      WaypointSettings         *wpSettings,
                      hydroqguielementutil::IHumanManager &humanManager );
    
    virtual ~PathPlannerInput() {};  
    
    virtual void paint( QPainter *painter ) {};
    virtual void setUseTransparency( bool useTransparency ) {};
        
    virtual void processPressEvent( QMouseEvent* e) {};
    virtual void processReleaseEvent( QMouseEvent* e ) {};
    virtual void processDoubleClickEvent( QMouseEvent* e) {};
    virtual void processMoveEvent( QMouseEvent* e) {};
    virtual void updateWpSettings( WaypointSettings* wpSettings ) {};
        
    virtual void savePath( const QString &filename ) {};
    virtual void loadPath( const QString &filename ) {};
    virtual void loadPreviousPath() {};
    virtual void sendPath() {};
    virtual void cancelPath() {};
    
    virtual void setWaypointFocus( int waypointId ) {};
    
    orca::PathPlanner2dTask getTask() const;
            
    private:
        std::auto_ptr<GuiPath> guiPath_;
        std::auto_ptr<PathDesignScreen> pathDesignScreen_;

};

}

#endif
