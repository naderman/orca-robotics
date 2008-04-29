/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QORC_PATHPAINTER_H
#define QORC_PATHPAINTER_H


#include <QPolygon>
#include <QColor>

#include <orcaqgui2dfactory/pathinput.h>
#include <hydroqguielementutil/definitions2d.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {
    
class PathPainter
{

  public:
    PathPainter();
    ~PathPainter();
    void initialize( bool displayWaypoints, bool displayPastWaypoints, bool displayFutureWaypoints, bool displayOlympicMarker, bool useTransparency);
    void setData( const orca::PathFollower2dData& path );
    void setData( const orca::PathPlanner2dData& path );
    void setWpIndex( int index );
    void setRelativeStartTime( double relativeStartTime );
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const { return z==hydroqguielementutil::Z_PATH; };
    void setUseTransparency( bool useTransparency ) { useTransparency_= useTransparency; };

    void clear();
    void togglePastWaypoints()
        { displayPastWaypoints_ = !displayPastWaypoints_; }
    void toggleFutureWaypoints()
        { displayFutureWaypoints_ = !displayFutureWaypoints_; }
    
    void toggleDisplayWaypoints()
        { displayWaypoints_ = !displayWaypoints_; }
    
    void toggleOlympicMarker()
        { displayOlympicMarker_ = !displayOlympicMarker_; }
    
    void savePath( const QString fileName, hydroqguielementutil::IHumanManager *humanManager ) const;
    void setColor( QColor color ) { color_ = color; };
    void setFocus( bool inFocus ) { inFocus_  = inFocus; };
    
  private:

    GuiPath guiPath_;

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
