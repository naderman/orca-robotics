/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>

#include <QPainter>
#include <orcaobj/orcaobj.h>

#include "gpspainter.h"

using namespace orca; 
using namespace std;
using namespace orcaqgui;

GpsPainter::GpsPainter(const QColor & colour, const bool showHistory)
    : isDataAvailable_(false),
      basicColour_(colour),
      isDisplayHistory_(showHistory)
{
}

void
GpsPainter::clear()
{
    isDataAvailable_ = false;
}

void 
GpsPainter::setData( const float &x, const float &y, const float &heading )
{    
    // set local storage
    x_ = x;
    y_ = y;
    heading_ = (int)floor( RAD2DEG( heading ) );
    isDataAvailable_ = true;
    
    history_.addPoint( x, y );

    currentColour_ = basicColour_;
}

void GpsPainter::paint( QPainter *painter, int z )
{
    if ( !isDataAvailable_ ) return;
    
    if ( z == orcaqgui::Z_POSE-2 && isDisplayHistory_ ) {
        history_.paint( painter, currentColour_ );
    }
    
    if ( z == orcaqgui::Z_POSE ) 
    {
        painter->save();
        {
            painter->translate( x_, y_ );
            painter->rotate( heading_ );
            orcaqgui::paintPlatformPose( painter, currentColour_ );
        }
        painter->restore();
    }
}

