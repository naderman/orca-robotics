/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_LASERSCANNER2D_PAINTER_H
#define ORCAGUI3D_LASERSCANNER2D_PAINTER_H

#include <QColor>
#include <QGLWidget>
#include <orca/laserscanner2d.h>

namespace orcaqgui3d
{

/*!
  @brief LaserPainter paints laser scans
*/
class LaserScanner2dPainter
{
  
  public:
    // -1 means 'default'
    LaserScanner2dPainter( QColor outlineColor=QColor( 102,102,153, 255 ),
                    float  outlineThickness=-1,
                    float  brightReturnWidth=0.2 );
    ~LaserScanner2dPainter();

    void setDescription( const orca::RangeScanner2dDescription& descr );
    void setData( const orca::RangeScanner2dDataPtr &scan );
    
    void paint( QGLWidget *p );

    void clear();
    
    void execute( int action );
    
    void setColor( QColor color );
    void setFocus( bool inFocus );

  private:

    orca::LaserScanner2dDataPtr data_;
    orca::RangeScanner2dDescription description_;

    bool isDisplayScan_;
    // bool isDisplayWalls_;
    bool isDisplayReflectors_;
    QColor outlineColor_;
    QColor basisColor_;
    float  outlineThickness_;
    float  brightReturnWidth_;
};

} // namespace

#endif
