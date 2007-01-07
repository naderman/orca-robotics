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
GpsPainter::setData( const orca::GpsMapGridData& data )
{    
//     data_ = data;

    cout<<"TRACE(gpspainter.cpp): got data: " << orcaice::toString(data) << endl;
    // set local storage
    x_ = data.easting;
    y_ = data.northing;
    heading_ = (int)floor( RAD2DEG( data.heading ) );
    isDataAvailable_ = true;

    // should we keep history even if not displaying?
    if ( isDisplayHistory_ ) {
        // Pose2dHypothesis &h = orcaice::mlHypothesis( data );
        history_.addPoint( x_, y_ );
    }

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
       
//         for ( unsigned int i=0; i<data_->hypotheses.size(); ++i )
//         {
//             float weight = data_->hypotheses[i].weight;
// 
//             // Don't bother painting _really_ unlikely hypotheses
//             if ( weight < 0.02 ) continue;
// 
//             Frame2d      &mean = data_->hypotheses[i].mean;
//             Covariance2d &cov  = data_->hypotheses[i].cov;
// 
//             // Translate to where the hypothesis is at
//             {
//                 ScopedSaver translateSaver(p);
//                 painter->translate( mean.p.x, mean.p.y );
// 
//                 QColor colour = getTransparentVersion( currentColour_, weight );
// 
//                 // Rotate to draw the platform correctly
//                 {
//                     ScopedSaver rotateSaver(p);
//                     painter->rotate( RAD2DEG( mean.o ) );
//                     paintPlatformPose( p, 
//                                        colour );
//                 }
//
//                paintUncertaintyInfo( p,
//                                      colour,
//                                      mean.o,
//                                      cov.xx,
//                                      cov.xy,
//                                      cov.yy,
//                                     cov.tt );
//            }
//        }
    }
}

void
GpsPainter::execute( int action )
{
    switch ( action )
    {
    case 0 :
        // toggle history
        isDisplayHistory_ = !isDisplayHistory_;
        break;
    }
}
