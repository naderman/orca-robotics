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
#include <orcaqgui2dfactory/pathutils.h>

class QPainter;
class QMouseEvent;

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace orcaqgui2d {

//!
//! A class used to design paths by clicking on the screen
//!
//! @author Tobias Kaupp
//!  
class PathDesignScreen
{           
    public:
        
        PathDesignScreen( GuiPath                             &guiPath,
                          WaypointSettings                    *wpSettings,
                          hydroqguielementutil::IHumanManager &humanManager );  
     
        void paint( QPainter *painter );
        
        void setUseTransparency( bool useTransparency ) 
            { useTransparency_=useTransparency; };
        
        void processPressEvent( QMouseEvent* e);
        void processReleaseEvent( QMouseEvent* e );
        
        void updateWpSettings( WaypointSettings* wpSettings );
        void setWaypointFocus( int waypointId );
        
        // Return the time it takes to complete a loop given the current path
        // (last waypoint to first waypoint of path)
        float secondsToCompleteLoop() const;
        
        
    private:    
       
        GuiPath &guiPath_;
        WaypointSettings *wpSettings_;
        hydroqguielementutil::IHumanManager &humanManager_;
        
        QMatrix wmInv_; // win2mm matrix
        
        bool useTransparency_;
        
        void addWaypoint( QPointF );
        void removeWaypoint( QPointF ); 
        void changeWpParameters( QPointF );
        void resizeData( int index );
               
        QPointF mouseDownPnt_;
        QPointF mouseUpPnt_;
        
        int waypointInFocus_;
        
};

} // namespace

#endif
