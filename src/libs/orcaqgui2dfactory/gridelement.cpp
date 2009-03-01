/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath> // for ceil()

#include <hydroqguielementutil/paintutils.h>

#include "gridelement.h"

using namespace std;

namespace orcaqgui2d {

GridElement::GridElement() :
        step_( 1.0 ),   // [m]
        isDisplayGrid_(true),
        isDisplayOrigin_(true),
        isDisplayLabels_(true)
{
}

void
GridElement::paint( QPainter *painter, int z )
{
    QMatrix m = painter->worldMatrix();  
    const double radius  = 6.0/m.m11();
    const double lineThickness = 2.0/m.m11();
    
    if ( z==hydroqguielementutil::Z_ORIGIN && isDisplayOrigin_ ) {
        //cout<<"painting origin"<<endl;
        hydroqguielementutil::paintOrigin( painter, Qt::blue, radius, lineThickness );
        return;
    }

    if ( z!=hydroqguielementutil::Z_GRID || !isDisplayGrid_ ) return;
    
    // save painter settings before doing anything
    painter->save();

    // adjust the grid step size
    QLineF oneMeter = painter->matrix().map( QLineF( QPointF(0.0,0.0), QPointF(0.0,1.0) ) );
//     cout<<"one grid cell in pixels :"<<step_*oneMeter.length()<<endl;
    if ( step_*oneMeter.length()<20.0 ) {
        step_ *= 2.0;
    }
    else if ( step_*oneMeter.length()>50.0 ) {
        step_ *= 0.5;
    }
    
    // need current world matrix
    QMatrix mi = m.inverted();       // win2m
    QRect rwin = painter->window();       // rect. in window cs
    QRectF rm = mi.mapRect( QRectF(rwin) ); // rect. in m (world) cs
//     cout<<"rect [win] LR="<<rwin.left()<<":"<<rwin.right()<<" TB="<<rwin.top()<<":"<<rwin.bottom()<<endl;
//     cout<<"rect [m] LR="<<rm.left()<<":"<<rm.right()<<" TB="<<rm.top()<<":"<<rm.bottom()<<endl;
    
    // snap world rectangle to visible grid lines
    double gleft = step_* (int)ceil( rm.left()/step_ );
    double gright = step_ * (int)floor( rm.right()/step_ );
    double gtop = step_ * (int)ceil( rm.top()/step_ );
    double gbottom = step_ * (int)floor( rm.bottom()/step_ );
//     cout<<"debug: LR="<<gleft<<":"<<gright<<" TB="<<gtop<<":"<<gbottom<<endl;

    //
    // paint grid lines
    //
    painter->setPen( 	QColor(185,185,185) );
    
    for ( double x=gleft; x<=gright; x += step_ ) {
            painter->drawLine( QPointF(x,rm.top()), QPointF(x,rm.bottom()) );
    }
    for ( double y=gtop; y<=gbottom; y += step_ ) {
            painter->drawLine( QPointF(rm.left(),y), QPointF(rm.right(),y) );
    }

    //
    // paint axis labels
    //
    if ( isDisplayLabels_ ) {
        painter->setFont( QFont("Helvetica [Cronyx]", 12) );
        painter->setMatrix( QMatrix() );
        // don't label the first grids because they overlap each other
        for ( double x=gleft+step_; x<=gright; x += step_ ) {
            painter->drawText( QPointF(x,gbottom-step_) * m, QString::number( x ) );
        }
        for ( double y=gtop+step_; y<=gbottom; y += step_ ) {
            painter->drawText( QPointF(gleft,y) * m, QString::number( y ) );
        }
    }
    
    painter->restore();
}

QStringList
GridElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Grid"<<"Toggle Origin"<<"Toggle Labels";
    return s;
}

void
GridElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
        // toggle grid
        isDisplayGrid_ = !isDisplayGrid_;
        break;
    case 1 :
        // toggle origin
        isDisplayOrigin_ = !isDisplayOrigin_;
        break;
    case 2 :
        // toggle labels
        isDisplayLabels_ = !isDisplayLabels_;
        break;
    }
}

}
