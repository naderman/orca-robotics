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
#include <GL/glut.h>
#include <orcaqgui3d/glutil.h>
#include <orcaice/orcaice.h>

#include "laserscanner2dpainter.h"

using namespace std;
using namespace orca;
using namespace orcaqgui3d;

LaserScanner2dPainter::LaserScanner2dPainter( QColor outlineColor,
                                float  outlineThickness,
                                float  brightReturnWidth )
    : isDisplayScan_(true),
      // isDisplayWalls_(true),
      isDisplayReflectors_(true),
      outlineColor_(outlineColor),
      outlineThickness_(outlineThickness),
      brightReturnWidth_(brightReturnWidth)
{
}

LaserScanner2dPainter::~LaserScanner2dPainter()
{
}

void
LaserScanner2dPainter::clear()
{
//     data_ = 0;
}

void
LaserScanner2dPainter::setDescription( const orca::RangeScanner2dDescription& descr )
{
    description_ = descr;
}

void
LaserScanner2dPainter::setData( const orca::RangeScanner2dDataPtr & data )
{
//     if ( data==0 ) return;

    // Assume that this thing is really a laser data.
    LaserScanner2dDataPtr scan = LaserScanner2dDataPtr::dynamicCast( data );
        
    assert( scan && "check that data is actually a laser scan" );

    data_ = scan;

    // debug
    // std::cout << "laserpainter3d.cpp: " << orcaice::toString(data_) << std::endl;
}

void
LaserScanner2dPainter::paint( QGLWidget *p )
{
//     if ( description_ == 0 ) return;

    glutil::ScopedMatrixSave s;

    // Apply the vehicle-to-sensor transformation
    orcaqgui3d::glutil::transform(  description_.offset.p.x,
                                 description_.offset.p.y,
                                 description_.offset.p.z,
                                 RAD2DEG(description_.offset.o.y),
                                 RAD2DEG(description_.offset.o.p),
                                 RAD2DEG(description_.offset.o.r) );

    const bool drawSurfaces  = true;
    const bool drawWireFrame = true;
    glColor4f( 0, 0, 1, 0.3 );
    orcaqgui3d::glutil::drawBox( description_.size.l,
                              description_.size.w,
                              description_.size.h,
                              drawSurfaces, 
                              drawWireFrame );

//     if ( data_ == 0 ) return;

//    Polygon doesn't work so good...  slows things _right_ down.
//    glBegin( GL_POLYGON );  
    glBegin( GL_LINE_LOOP );  
    glColor4f( 1, 0, 0, 1 );
    glVertex3f( 0, 0, 0 );
    double angleIncrement = data_->fieldOfView / double(data_->ranges.size()+1);
    for ( unsigned int i=0; i < data_->ranges.size(); ++i )
    {
        float bearing = data_->startAngle + i * angleIncrement;
        glVertex3f( data_->ranges[i] * cos(bearing),
                    data_->ranges[i] * sin(bearing),
                    0 );
    }
    glEnd();  
}

void
LaserScanner2dPainter::execute( int action )
{
    switch ( action )
    {
    case 0 :
        // toggle data_
        isDisplayScan_ = !isDisplayScan_;
        break;
    case 1 :
        // toggle walls
        // isDisplayWalls_ = !isDisplayWalls_;
        break;
    case 2 :
        // toggle reflectors
        isDisplayReflectors_ = !isDisplayReflectors_;
        break;
    }
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
