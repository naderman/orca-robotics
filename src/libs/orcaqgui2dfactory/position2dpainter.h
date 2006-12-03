/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_POSITION2D_PAINTER_H
#define ORCAGUI_POSITION2D_PAINTER_H

#include <iostream>
#include <QColor>

#include <orca/position2d.h>
#include <orcaqgui2d/definitions2d.h>

#include <orcaqgui2d/paintutils.h>

class QPainter;

namespace orcaqgui
{

/*!
  @brief Paints current velocity.
*/
class Position2dPainter
{
  public:
    // The default color is the color when the robot is not selected.
    Position2dPainter( const QColor & colour=Qt::blue, const bool showHistory=true );
    ~Position2dPainter();
    
    void setData( const orca::Position2dDataPtr & posData );
    void paint( QPainter *painter, const int z );
    bool paintThisLayer(int z) const { return z==Z_POSE; }
    void clear();
    
    void setFocus( bool focus );
    
  private:

    // pose [m,m,deg]
    double x_;
    double y_;
    int heading_;
    
    bool isInFocus_;

    QColor basicColour_;
    QColor currentColour_;

    bool isDisplayHistory_;
    PoseHistory history_;
};

} // namespace

#endif
