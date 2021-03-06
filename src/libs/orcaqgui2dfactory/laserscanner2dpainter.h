/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_LASERSCANNER2D_PAINTER_H
#define ORCAGUI_LASERSCANNER2D_PAINTER_H

#include <QColor>
#include <QPolygonF>
#include <orca/laserscanner2d.h>
#include <hydroqguielementutil/definitions2d.h>

class QPainter;

namespace orcaqgui2d
{

/*!
  @brief Paints laser scans.
*/
class LaserScanner2dPainter
{
  
  public:
    // -1 means 'default'
    LaserScanner2dPainter( QColor outlineColor=QColor( 102,102,153, 255 ),
                           float  outlineThickness=-1,
                           float  brightReturnWidth=0.2 );

    void setOffset( orca::Frame3d &offset );
    void setData( const orca::RangeScanner2dDataPtr &scan );
    
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==hydroqguielementutil::Z_LASER;}

    void clear();
    
    void execute( int action );
    
    void setLaserMaxRange( float laserMaxRange ) { laserMaxRange_ = laserMaxRange; }
    
    void setColor( QColor color );
    void setFocus( bool inFocus );

  private:

    std::vector<float>         ranges_;
    std::vector<unsigned char> intensities_;
    bool                       intensitiesValid_;

    // The scan (set of points for drawing the scan outline)
    QPolygonF qScan_;
    // The actual returns (doesn't include a point at the origin)
    QPolygonF qReturns_;
    
    double laserMaxRange_;

    bool isDisplayScan_;
    bool isDisplayPoints_;
    bool isDisplayWalls_;
    bool isDisplayReflectors_;
    bool isFilledPolygon_;
    QColor outlineColor_;
    QColor fillColor_;
    QColor basisColor_;
    float  outlineThickness_;
    float  brightReturnWidth_;

    double offsetX_;
    double offsetY_;
    double offsetYaw_;
	double offsetPitch_;

    bool isUpsideDown_;
};

} // namespace

#endif
