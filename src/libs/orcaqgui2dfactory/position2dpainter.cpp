/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <QPainter>

#include <orcaobj/mathdefs.h>

#include "position2dpainter.h"

using namespace orca; 
using namespace std;
using namespace orcaqgui;

Position2dPainter::Position2dPainter( const QColor &basicColour, const bool displayHistory )
    : x_(0.0),
      y_(0.0),
      heading_(0),
      isInFocus_(true),
      basicColour_(basicColour),
      isDisplayHistory_(displayHistory)
{
}

Position2dPainter::~Position2dPainter()
{
}

void
Position2dPainter::clear()
{
    x_ = 0.0;
    y_ = 0.0;
    heading_ = 0;
}

void
Position2dPainter::setData( const orca::Position2dDataPtr & data )
{
    if ( data==0 ) return;

    // set local storage
    x_ = data->pose.p.x;
    y_ = data->pose.p.y;
    heading_ = (int)floor( RAD2DEG( data->pose.o ) );
    //std::cout << "Position2dPainter: x,y: " << x_<< ", " << y_ <<std::endl;

    // should we keep history even if not displaying?
    if ( isDisplayHistory_ ) {
        history_.addPoint( x_, y_ );
    }

    if (isInFocus_) {
        currentColour_ = basicColour_;
    } else {
        currentColour_ = Qt::gray;
    }
}

void Position2dPainter::paint( QPainter* p, const int z )
{
//     if ( z == orcaqgui::Z_POSE-1 ) 
//     {
//         paintOrigin( p, currentColour_ );
//         history_.paint( p, currentColour_ );
//     }
    
    if ( z == orcaqgui::Z_POSE )
    {
        p->save();
        {
            p->translate( x_, y_ );
            p->rotate( heading_ );
            orcaqgui::paintPlatformPose( p, currentColour_ );
        }
        p->restore();
    }
}

void Position2dPainter::setFocus( bool focus )
{
    //cout << "TRACE(globalposepainter.cpp): setFocus " << focus << endl;
    isInFocus_ = focus;
}
