/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAGUI_GPS_PAINTER_H
#define ORCAGUI_GPS_PAINTER_H

#include <orcaqgui2d/definitions2d.h>

#include <orcaqgui2d/paintutils.h>

// forward declarations
class QPainter;
class QColor;

namespace orcaqgui
{

class GpsPainter
{
  public:
    // The default color is the color when the robot is not selected.
    GpsPainter( const QColor & color=Qt::blue, const bool showHistory=true );

    void setData( const double &x, const double &y, const double &heading, const double &horizontalError );

    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const {return z==Z_POSE || z==Z_POSE-2;}
    void clear();
    void setColor( QColor color ) { basicColor_ = color; currentColor_ = color; };
    void setFocus (bool inFocus );

    void toggleDisplayHistory() { isDisplayHistory_ = !isDisplayHistory_; }

  private:

    bool isDataAvailable_;

    QColor basicColor_;
    QColor currentColor_;

    bool isDisplayHistory_;
    PoseHistory history_;
    
    // pose [m,m,deg]
    double x_;
    double y_;
    int heading_;
    // corresponds to one standard deviation
    double horizontalError_;
};

}

#endif
