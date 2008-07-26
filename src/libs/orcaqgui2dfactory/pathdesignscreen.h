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
#include "pathutils.h"

namespace hydroqguielementutil {
    class IHumanManager;
}

namespace orcaqgui2d {

class PathDesignScreen
{           
    public:
        PathDesignScreen( GuiPath &guiPath,
                          WaypointSettings *wpSettings,
                          hydroqguielementutil::IHumanManager &humanManager );  
     
        void paint( QPainter *painter );
        void setUseTransparency( bool useTransparency ) { useTransparency_=useTransparency; };
        
        void processPressEvent( QMouseEvent* e);
        void processReleaseEvent( QMouseEvent* e );
        
        void updateWpSettings( WaypointSettings* wpSettings );
        void setWaypointFocus( int waypointId );
        
        float secondsToCompleteLoop() const;
        
    protected:    
        
        // The path in Gui representation. Shared with wpWidget
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
        QPointF mouseMovePnt_;
        QPointF doubleClick_;
        
        int waypointInFocus_;
        
};

} // namespace

#endif
