/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_LASERSCANNER2D_PAINTER_H
#define ORCAGUI3D_LASERSCANNER2D_PAINTER_H

#include <QColor>
#include <QGLWidget>
#include <orcaqgui3d/view.h>
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
    LaserScanner2dPainter( QColor outlineColor=QColor( 102,102,153, 255 ) );
//                            float  outlineThickness=-1,
//                            float  brightReturnWidth=0.2 );

    void setOffset( const orca::Frame3d &offset );
    void setSize( const orca::Size3d &size );
    void setData( const orca::RangeScanner2dDataPtr &scan );

    void paint( const orcaqgui3d::View &view, QGLWidget &p );

    void clear();
    
    void setLaserMaxRange( float laserMaxRange ) { laserMaxRange_ = laserMaxRange; }
    
    void setColor( QColor color );
    void setFocus( bool inFocus );

  private:

    std::vector<float>         ranges_;
    std::vector<unsigned char> intensities_;
    bool                       intensitiesValid_;

    double laserMaxRange_;
    double fieldOfView_;
    double startAngle_;

    bool isDisplayScan_;
    // bool isDisplayWalls_;
    bool isDisplayReflectors_;
    QColor outlineColor_;
    QColor basisColor_;

//     float  outlineThickness_;
//     float  brightReturnWidth_;

    double offsetX_;
    double offsetY_;
    double offsetZ_;
    double offsetRoll_;
	double offsetPitch_;
    double offsetYaw_;

    orca::Size3d size_;
};

} // namespace

#endif
