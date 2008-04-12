/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <gbxsickacfr/gbxutilacfr/mathdefs.h>
#include <hydroqguielementutil/paintutils.h>

#include "odometry2dpainter.h"

using namespace std;

namespace orcaqgui2d
{

Odometry2dPainter::Odometry2dPainter()
    : currentColour_(Qt::red)
{
}

Odometry2dPainter::~Odometry2dPainter()
{
}

void 
Odometry2dPainter::setColor( QColor color )
{
    currentColour_ = color;
}

void 
Odometry2dPainter::setData( const orca::Odometry2dData& data )
{
    speedX_ = data.motion.v.x;
    speedY_ = data.motion.v.y;
    turnrate_ = data.motion.w;
}

void Odometry2dPainter::paint( QPainter *p, int z )
{
    if (!paintThisLayer(z)) return;
    p->setPen( QPen( currentColour_ ) );
    
    const double ARROWHEAD_LENGTH = 0.5;        // in m
    const double ARROWHEAD_WIDTH = 0.2;
    const double ARCBOX_LENGTH = 2.0;
    const double treshholdSpeed = 0.05;         // in m/s
    const double treshholdTurnrate = 0.005;     // in rad/s
    
    QPointF p0(0.0, 0.0);
    QPointF p1, p2;
    double arrowLength = 0.0;

    p->save();
    {
        p->rotate(-90.0);
        
        // Paint speed arrow
        p->setPen( QPen( hydroqguielementutil::getDarkVersion(currentColour_), 0.1 ) );
        p->save(); //  frame
        arrowLength = speedX_*5.0;
        p->drawLine( QPointF(0.0, 0.0), QPointF(0.0, arrowLength) );
        p->translate ( 0.0, arrowLength );
        if (arrowLength > treshholdSpeed)
        {
            p1.setX( -ARROWHEAD_WIDTH );
            p1.setY( -ARROWHEAD_LENGTH );
            p2.setX( ARROWHEAD_WIDTH );
            p2.setY( -ARROWHEAD_LENGTH );
        }
        else if  (arrowLength < -treshholdSpeed)
        {
            p1.setX( -ARROWHEAD_WIDTH );
            p1.setY( ARROWHEAD_LENGTH );
            p2.setX( ARROWHEAD_WIDTH );
            p2.setY( ARROWHEAD_LENGTH );   }
        else
        {
            p1.setX( 0.0 );
            p1.setY( 0.0 );
            p2.setX( 0.0 );
            p2.setY( 0.0 );
        }

        p->drawLine( p0, p1 );
        p->drawLine( p0, p2 );
        p->restore();  // back to frame
        
        // Paint turnrate arrow
        p->save(); //  frame

        if (turnrate_< -treshholdTurnrate)
        {
            p->drawArc(  QRectF(-ARCBOX_LENGTH, -ARCBOX_LENGTH, 2.0*ARCBOX_LENGTH, 2.0*ARCBOX_LENGTH),
                         (int)floor(RAD2DEG(turnrate_))/2 * 16,
                         -(int)floor(RAD2DEG(turnrate_))*16 );
            p->rotate( RAD2DEG(turnrate_)/2 );
            p->translate( ARCBOX_LENGTH, 0.0 );
            p1.setX( -ARROWHEAD_WIDTH );
            p1.setY( ARROWHEAD_LENGTH );
            p2.setX( ARROWHEAD_WIDTH );
            p2.setY(  ARROWHEAD_LENGTH );
        }
        else if (turnrate_ > treshholdTurnrate)
        {
            p->drawArc(  QRectF(-ARCBOX_LENGTH, -ARCBOX_LENGTH, 2.0*ARCBOX_LENGTH, 2.0*ARCBOX_LENGTH),
                         (int)floor(-180.0+RAD2DEG(turnrate_)/2) * 16,
                         -(int)floor(RAD2DEG(turnrate_))*16 );
            p->rotate( RAD2DEG(turnrate_)/2 );
            p->translate( -ARCBOX_LENGTH, 0.0 );
            p1.setX( -ARROWHEAD_WIDTH );
            p1.setY( ARROWHEAD_LENGTH );
            p2.setX( ARROWHEAD_WIDTH );
            p2.setY(  ARROWHEAD_LENGTH );
        }
        else
        {
            p1.setX( 0.0 );
            p1.setY( 0.0 );
            p2.setX( 0.0 );
            p2.setY( 0.0 );
        }

        p->drawLine( p0, p1 );
        p->drawLine( p0, p2 );

        p->restore();  // back to frame
    }
    p->restore();
}

} // namespace

