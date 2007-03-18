/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef QORC_PATHPAINTER_H
#define QORC_PATHPAINTER_H


#include <qpolygon.h>
#include <QVector>
#include <QColor>

#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2d/definitions2d.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {
    
class PathPainter
{

  public:
    PathPainter();
    ~PathPainter();
    void initialize( bool displayWaypoints, bool displayPastWaypoints, bool useTransparency);
    void setData( const orca::PathFollower2dData& path );
    void setData( const orca::PathPlanner2dData& path );
    void setWpIndex( int index );
    void setRelativeStartTime( double relativeStartTime );
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const { return z==Z_PATH; };
    void setTransparency( bool useTransparency ) { useTransparency_= useTransparency; };

    void clear();
    void togglePastWaypoints()
        { displayPastWaypoints_ = !displayPastWaypoints_; }
    
    void toggleDisplayWaypoints()
        { displayWaypoints_ = !displayWaypoints_; }
    
    int savePath( const QString fileName, orcaqgui::IHumanManager *humanManager ) const;
    void setColor( QColor color ) { color_ = color; };
    void setFocus( bool inFocus ) { inFocus_  = inFocus; };
    
  private:

    QPolygonF      waypoints_;           // in m
    QVector<int>   headings_;            // in 1/16 deg from 0 to 360*16
    QVector<float> distTolerances_;      // distance tolerances in m
    QVector<int>   headingTolerances_;   // heading tolerances in 1/16 deg from 0 to 360*16
    
    // Currently there's no graphical representation of these values
    // We need to store them because the painter also saves the data to files
    QVector<float> times_;               // number of seconds until arrival at waypoint
    QVector<float> maxSpeeds_;           // max speed in m/s
    QVector<int> maxTurnrates_;          // max turnrate in deg/s

    // The index of the waypoint we're currently pursuing
    int wpIndex_;

    // toggle states
    bool displayWaypoints_;
    bool displayPastWaypoints_;
    bool useTransparency_;
    
    QColor color_;
    bool inFocus_;
    
    double relativeStartTime_;

    void setDataLocal( orca::Path2d & path );
};

}

#endif
