/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_IPATH_INPUT_H
#define ORCA_IPATH_INPUT_H

#include <memory>

class QMouseEvent;
class QPainter;

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace orca {
    class PathFollower2dData;
}

namespace orcaqgui2d {

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
        virtual void savePath( const QString &filename ) = 0;
        virtual void loadPath( const QString &filename ) = 0;
        virtual void loadPreviousPath() = 0;
        
        // send and cancel path
        virtual void sendPath() = 0;
        virtual void cancelPath() = 0;
        virtual void setWaypointFocus( int waypointId ) = 0;
        
        // This is not fully virtual but avoids having to inherit from PathFollower2dData
        virtual bool getPath( orca::PathFollower2dData &pathData ) const { return false; };
        
};

//!
//! A factory that can create PathFollowerInput objects at runtime
//!
//! @author Tobias Kaupp
//!
class PathFollowerInputFactory
{
    public:
    
        virtual std::auto_ptr<IPathInput> 
            createPathFollowerInput( PathFollowerUserInteraction        &pathFollowerUI,
                                    WaypointSettings                    *wpSettings,
                                    hydroqguielementutil::IHumanManager &humanManager,
                                    const QString                       &lastSavedPathFile ) const = 0;
};



}

#endif
