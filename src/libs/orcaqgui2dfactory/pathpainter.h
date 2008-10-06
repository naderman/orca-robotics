/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPAINTER_H
#define PATHPAINTER_H

#include <QColor>
#include <hydroqguipath/pathutils.h>
#include <hydroqguielementutil/definitions2d.h>
#include <orca/pathplanner2d.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {
    
class PathPainter
{

  public:
    PathPainter();
    void initialize( bool displayWaypoints, 
                     bool displayPastWaypoints, 
                     bool displayFutureWaypoints, 
                     bool displayOlympicMarker, 
                     bool useTransparency);
    
    void setData( const orca::PathFollower2dData& path );
    
    void setData( const orca::PathPlanner2dData& path );
    
    void setWpIndex( int index );
    
    void setRelativeStartTime( double relativeStartTime );
    
    void paint( QPainter *p, int z );
    
    bool paintThisLayer(int z) const 
        { return z==hydroqguielementutil::Z_PATH; };
    
    void setUseTransparency( bool useTransparency ) 
        { useTransparency_= useTransparency; };

    void clear();
    
    void togglePastWaypoints()
        { displayPastWaypoints_ = !displayPastWaypoints_; }
    
    void toggleFutureWaypoints()
        { displayFutureWaypoints_ = !displayFutureWaypoints_; }    
    
    void toggleDisplayWaypoints()
        { displayWaypoints_ = !displayWaypoints_; }

    void setDisplayPastWaypoints( bool display )   
        { displayPastWaypoints_ = display; }
    
    void setDisplayFutureWaypoints( bool display ) 
        { displayFutureWaypoints_ = display; }
    
    void setDisplayWaypoints( bool display )       
        { displayWaypoints_ = display; }
    
    void toggleOlympicMarker()
        { displayOlympicMarker_ = !displayOlympicMarker_; }

    void setColor( QColor color ) 
        { color_ = color; };
    
    void setFocus( bool inFocus ) 
        { inFocus_  = inFocus; };
    
    const hydroqguipath::GuiPath &currentPath() const 
        { return guiPath_; }; 
    
  private:

    hydroqguipath::GuiPath guiPath_;

    // The index of the waypoint we're currently pursuing
    int wpIndex_;

    // toggle states
    bool displayWaypoints_;
    bool displayPastWaypoints_;
    bool displayFutureWaypoints_;
    bool displayOlympicMarker_;
    bool useTransparency_;
    
    QColor color_;
    bool inFocus_;
    
    double relativeStartTime_;

};

}

#endif
