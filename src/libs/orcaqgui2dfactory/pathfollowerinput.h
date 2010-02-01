/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER_INPUT_H
#define PATHFOLLOWER_INPUT_H

#include <hydroqguipath/pathdesignscreen.h>
#include <hydroqguipath/pathdesigntablewidget.h>
#include <hydroqguipath/ipathinput.h>
#include <hydroqguipath/pathutils.h>
#include <orcaqgui2dfactory/pathfilehandler.h>

namespace orcaqgui2d {
    
class PathFollowerUserInteraction;

//!
//! An implementation of PathInput for the PathFollower
//! Enables user to click on screen and manipulate path using a table
//!
//! @author Tobias Kaupp
//!
class PathFollowerInput : public hydroqguipath::IPathInput
{ 
            
    public:
        PathFollowerInput( hydroqguipath::IPathUserInteraction  *pathUI,
                           hydroqguipath::WaypointSettings      *wpSettings,
                           hydroqguielementutil::IHumanManager  &humanManager );
        
        virtual ~PathFollowerInput();  
  
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
        
        virtual void getPath( hydroqguipath::GuiPath &guiPath, int &numLoops, float &timeOffset ) const;

        virtual void switchTableView( bool onOff );
        
    private:
        
        hydroqguipath::IPathUserInteraction *pathUI_;
        std::auto_ptr<hydroqguipath::PathDesignScreen> pathDesignScreen_;
        std::auto_ptr<hydroqguipath::PathDesignTableWidget> pathDesignTableWidget_;
        std::auto_ptr<hydroqguipath::GuiPath> guiPath_;
       
};

//!
//! A factory used to create a PathFollowerInput object at runtime
//!
class PathFollowerInputFactory : public hydroqguipath::PathInputFactory
{
public:
    
    virtual std::auto_ptr<hydroqguipath::IPathInput> 
        createPathInput( hydroqguipath::IPathUserInteraction  *pathUI,
                         hydroqguipath::WaypointSettings      *wpSettings,
                         hydroqguielementutil::IHumanManager  &humanManager ) const
        {
            std::auto_ptr<hydroqguipath::IPathInput> input;
            input.reset( new PathFollowerInput( pathUI, wpSettings, humanManager ) );
            return input;     
        }

    virtual ~PathFollowerInputFactory() {}
};      

}

#endif
