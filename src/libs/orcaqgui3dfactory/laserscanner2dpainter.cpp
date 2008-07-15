/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <assert.h>
#include <QString>
#include <orcaqgui3d/glutil.h>
#include <orcaice/orcaice.h>

#include "laserscanner2dpainter.h"

using namespace std;
using namespace orca;
using namespace orcaqgui3d;

LaserScanner2dPainter::LaserScanner2dPainter( QColor outlineColor )
//                                 float  outlineThickness,
//                                 float  brightReturnWidth )
    : isDisplayScan_(true),
      // isDisplayPoints_(false),
      // isDisplayWalls_(true),
      isDisplayReflectors_(true),
      outlineColor_(outlineColor),
//       outlineThickness_(outlineThickness),
//       brightReturnWidth_(brightReturnWidth),
      offsetX_(0.0),
      offsetY_(0.0),
      offsetZ_(0.0),
      offsetRoll_(0.0),
      offsetPitch_(0.0),
      offsetYaw_(0.0)
{
}

void
LaserScanner2dPainter::clear()
{
    ranges_.clear();
}

void
LaserScanner2dPainter::setOffset( const orca::Frame3d &offset )
{
    offsetX_     = offset.p.x;
    offsetY_     = offset.p.y;
    offsetZ_     = offset.p.z;
    offsetRoll_  = offset.o.r;
    offsetPitch_ = offset.o.p;
    offsetYaw_   = offset.o.y;
}

void
LaserScanner2dPainter::setSize( const orca::Size3d &size )
{
    size_ = size;
}

void
LaserScanner2dPainter::setData( const orca::RangeScanner2dDataPtr & data )
{
    if ( data==0 ) return;

    // Check if this thing is a laser scan.
    orca::LaserScanner2dDataPtr laserScan = orca::LaserScanner2dDataPtr::dynamicCast( data );
    if ( laserScan )
    {
        intensitiesValid_ = true;
        intensities_      = laserScan->intensities;        
    }

    ranges_        = data->ranges;
    laserMaxRange_ = data->maxRange;
    fieldOfView_   = data->fieldOfView;
    startAngle_    = data->startAngle;
}

void
LaserScanner2dPainter::paint( const orcaqgui3d::View &view, QGLWidget &p )
{
    // Apply the vehicle-to-sensor transformation
    orcaqgui3d::glutil::transform( offsetX_, offsetY_, offsetZ_,
                                   offsetRoll_, offsetPitch_, offsetYaw_ );

    const bool drawSurfaces  = true;
    const bool drawWireFrame = true;
    glColor3f( 0, 0, 1 );
    orcaqgui3d::glutil::drawBox( size_.l,
                                 size_.w,
                                 size_.h,
                                 drawSurfaces, 
                                 drawWireFrame );
    
    if ( !isDisplayScan_ ) return;
    if ( ranges_.size() == 0 ) return;

//    Polygon doesn't work so good...  slows things _right_ down.
//    glBegin( GL_POLYGON );  
    glBegin( GL_LINE_LOOP );  
    glColor3f( outlineColor_.red(), outlineColor_.green(), outlineColor_.blue() );
    glVertex3f( 0, 0, 0 );
    double angleIncrement = fieldOfView_ / double(ranges_.size()+1);
    for ( unsigned int i=0; i < ranges_.size(); ++i )
    {
        float bearing = startAngle_ + i * angleIncrement;
        glVertex3f( ranges_[i] * cos(bearing),
                    ranges_[i] * sin(bearing),
                    0 );
    }
    glEnd();  
}

void LaserScanner2dPainter::setColor( QColor color )
{ 
    basisColor_ = color; 
    outlineColor_ = color;
}

void LaserScanner2dPainter::setFocus( bool inFocus )
{
    if (!inFocus) {
        outlineColor_=Qt::gray;
    } else {
        outlineColor_=basisColor_;
    }    
}
