/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNERPAINTER_H
#define PATHPLANNERPAINTER_H

#include <QColor>
#include <hydroqguipath/pathutils.h>
#include <hydroqguielementutil/definitions2d.h>

// forward declarations
class QPainter;

namespace orca {
    class PathPlanner2dData;
}

namespace orcaqgui2d {

    
class PathPlannerPainter
{

  public:
    PathPlannerPainter();
    
    void initialize( bool useTransparency, bool displayWaypoints )
        { useTransparency_ = useTransparency;
          displayWaypoints_ = displayWaypoints;
        }
    
    void setData( const orca::PathPlanner2dData& path );

    void paint( QPainter *p, int z );
    
    bool paintThisLayer(int z) const 
        { return z==hydroqguielementutil::Z_PATH; }

    void clear()
        { guiPath_.resize(0); }

    void toggleDisplayWaypoints()
        { displayWaypoints_ = !displayWaypoints_; }

    void setDisplayWaypoints( bool display )
        { displayWaypoints_ = display; }

    bool displayWaypoints()
        { return displayWaypoints_; }

    void setUseTransparency( bool useTransparency )
        { useTransparency_ = useTransparency; };

    bool useTransparency()
        { return useTransparency_; };

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

    bool useTransparency_;
    bool displayWaypoints_;
    
    QColor color_;
    bool inFocus_;
    

};

}

#endif
