/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef IPATH_INPUT_H
#define IPATH_INPUT_H

#include <memory>
#include "pathutils.h"

class QMouseEvent;
class QPainter;

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace orca {
    class PathFollower2dData;
}

namespace hydroqguipath {

class WaypointSettings;
class PathFollowerUserInteraction;
    
//!
//! Abstract class for user path input
//!
//! @author Tobias Kaupp
//!
class IPathInput
{   
              
public:
    virtual ~IPathInput() {};  
    
    // painting
    virtual void paint( QPainter *painter ) = 0;
    virtual void setUseTransparency( bool useTransparency ) = 0;
    
    // mouse events
    virtual void processPressEvent( QMouseEvent* e) = 0;
    virtual void processReleaseEvent( QMouseEvent* e ) = 0;
    virtual void processDoubleClickEvent( QMouseEvent* e) = 0;
    virtual void processMoveEvent( QMouseEvent* e) = 0;
    
    virtual void updateWpSettings( WaypointSettings* wpSettings ) = 0;
    
    // save and load paths
    virtual void savePath() = 0;
    virtual void loadPath() = 0;
    virtual void loadPreviousPath() = 0;
    
    // send and cancel path
    virtual void sendPath() = 0;
    virtual void cancelPath() = 0;
    virtual void setWaypointFocus( int waypointId ) = 0;
    
    // get the gui path
    virtual void getPath( hydroqguipath::GuiPath &guiPath, int &numLoops, float &timeOffset ) const = 0;

    // table view
    virtual void switchTableView( bool onOff ) = 0;

};

class IPathUserInteraction
{
public:
    virtual ~IPathUserInteraction() {};
    virtual void send() = 0;
    virtual void cancel() = 0;  
    virtual void saveUserPath( const hydroqguipath::GuiPath &guiPath,
                               int numLoops = 1,
                               float timeOffset = 0.0 ) = 0;
    virtual void loadUserPath(hydroqguipath::GuiPath &guiPath) = 0;
    virtual void loadPreviousUserPath(hydroqguipath::GuiPath &guiPath) = 0;    
};

//!
//! Abstract factory class for pathinput object creation at run-time
//!
//! @author Tobias Kaupp
//!
class PathInputFactory
{
    
public:

    virtual std::auto_ptr<IPathInput> 
        createPathInput( IPathUserInteraction                *pathUI,
                         WaypointSettings                    *wpSettings,
                         hydroqguielementutil::IHumanManager &humanManager
                       ) const = 0;
    
    virtual ~PathInputFactory() {}
};



}

#endif
