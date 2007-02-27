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
#include <QColor>
#include <orcaobj/orcaobj.h>

#include "gpspainter.h"

using namespace orca; 
using namespace std;
using namespace orcaqgui;

GpsPainter::GpsPainter(const QColor & color, const bool showHistory)
    : isDataAvailable_(false),
      basicColor_(color),
      isDisplayHistory_(showHistory)
{
}

void
GpsPainter::clear()
{
    isDataAvailable_ = false;
}

void 
GpsPainter::setData( const double &x, 
                     const double &y, 
                     const double &heading, 
                     const double &horizontalError  )
{    
    // set local storage
    x_ = x;
    y_ = y;
    heading_ = (int)floor( RAD2DEG( heading ) );
    horizontalError_ = horizontalError;
    
    isDataAvailable_ = true;
    
    history_.addPoint( x, y );
}

void GpsPainter::paint( QPainter *painter, int z )
{
    if ( !isDataAvailable_ ) return;
    
    if ( z == orcaqgui::Z_POSE-2 && isDisplayHistory_ ) {
        history_.paint( painter, currentColor_ );
    }
    
    if ( z == orcaqgui::Z_POSE ) 
    {
        QMatrix m2win = painter->worldMatrix();
        painter->save();
        {
            painter->translate( x_, y_ );
            double covH = horizontalError_*horizontalError_;
            painter->rotate( heading_ );
            orcaqgui::paintPlatformPose( m2win, painter, currentColor_ );
            if (covH > 0.0)
                orcaqgui::paintCovarianceEllipse( m2win, painter, currentColor_, covH, 0.0, covH);
        }
        painter->restore();
    }
}

void GpsPainter::setFocus (bool inFocus )
{    
    if (!inFocus) {
        currentColor_ = Qt::gray;
    } else {
        currentColor_ = basicColor_;
    }  
    
}

