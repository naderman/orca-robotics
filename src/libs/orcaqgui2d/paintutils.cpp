/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaobj/mathdefs.h>
#include <iostream>
#include <sstream>
#include <orcaqgui/exceptions.h>
#include <orcaqgui/cov2d.h>
#include "paintutils.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

// STATIC VARIABLES

// Robot size [m]
const double FRAME_LENGTH = 0.220;
const double FRAME_WIDTH = 0.190;

// How much uncertainty to draw
const int N_SIGMA = 3;
const int N_LENGTH = 3;

// How thick to paint thin lines? [m]
const double THIN_LINE_THICKNESS = 0.05;
    
////////////////////////////////////////////////////////////////////////////////

namespace orcaqgui {

PoseHistory::PoseHistory( double lineThickness )
    : lineThickness_(lineThickness) {}
    
void
PoseHistory::addPoint( const double x, const double y )
{
    if ( history_.isEmpty() ) {
        history_.push_back( QPointF(x,y) );
        return;
    }
    
    const QPointF last = history_.at( history_.size()-1 );
    double dist = hypotf( last.x()-x, last.y()-y );
    
    if ( dist > _historyMinDistance )
    {
        history_.push_back( QPointF(x,y) );

        if ( history_.size() >= _historyMaxLength ) {
            history_.remove( 0,_historyMaxLength-_historyResetLength );
        }
    }
}

void
PoseHistory::paint( QPainter *p, const QColor &colour )
{
    QColor historyColour = getTransparentVersion( colour );

    // paint robot motion history
    if ( history_.size() > 1 )
    {
        p->setPen( QPen( historyColour, lineThickness_ ) );
        p->drawPolyline( history_ );
    }
}

void
paintOrigin( QPainter *p, QColor colour )
{
    p->save();
    
    p->setBrush( QBrush() );
    p->setPen( QPen( colour, THIN_LINE_THICKNESS ) );

    // display origin
    const double radius  = 0.300;    // [m]
    const double cross = 0.500;      // [m]
    p->drawEllipse( QRectF(-radius,-radius, 2.0*radius, 2.0*radius) );
    p->drawLine( QPointF(0.0,0.0), QPointF(cross,0.0) );
    p->drawLine( QPointF(0.0,0.0), QPointF(0.0,cross) );
    
    p->setFont( QFont("Helvetica [Cronyx]", 12) );
    
    QMatrix m = p->matrix();  // this is m2win matrix
    QPointF xlabel = QPointF(cross,0.0) * m;       // x-label position in window cs
    QPointF ylabel = QPointF(0.0,cross) * m;       // y-label position in window cs

    p->setMatrix( QMatrix() );
    
    p->drawText( xlabel, "x" );
    p->drawText( ylabel, "y" );
    
    p->restore();
}

void
paintPlatformPose( QPainter *p, QColor colour, float transparencyMultiplier )
{
    // paint body
    p->setBrush( getTransparentVersion(colour) );
    // The outline
    p->setPen( QPen( getTransparentVersion(Qt::black), THIN_LINE_THICKNESS ) );
    p->drawRect( QRectF( -FRAME_LENGTH, -FRAME_WIDTH, 2.0*FRAME_LENGTH,2.0*FRAME_WIDTH) ); // top,left, width,height

//    p->setPen( Qt::white );
    p->drawLine( QPointF(0.0,0.0), QPointF(FRAME_LENGTH,FRAME_WIDTH) );
    p->drawLine( QPointF(0.0,0.0), QPointF(FRAME_LENGTH,-FRAME_WIDTH) );
}

void
paintUncertaintyInfo( QPainter *p, QColor colour, float thetaMean, float pxx, float pxy, float pyy, float ptt )
{
    paintHeadingUncertainty( p, colour, thetaMean, ptt );
    paintCovarianceEllipse( p, colour, pxx, pxy, pyy );
}

void
paintHeadingUncertainty( QPainter *p, QColor colour, float thetaMean, float ptt )
{
    p->save();
    {
        p->rotate( thetaMean );
        // paint heading uncertainty
        p->setPen( QPen(colour, THIN_LINE_THICKNESS) );
        p->setBrush( Qt::NoBrush );
        int pTheta = (int) (ptt * (180.0/M_PI));
        p->drawPie( QRectF(-N_LENGTH*FRAME_LENGTH,-N_LENGTH*FRAME_LENGTH,
                    2.0*N_LENGTH*FRAME_LENGTH,2.0*N_LENGTH*FRAME_LENGTH),
                    -N_SIGMA*pTheta*16, 2*N_SIGMA*pTheta*16 );
    }
    p->restore();
}

void
paintCovarianceEllipse( QPainter *p, QColor colour, float pxx, float pxy, float pyy )
{
    // Quick checks first (note that this is a necessary but not
    // sufficient condition for positive-definiteness)
    if ( pxx < 0.0 ||
         pyy < 0.0 ||
         fabs(pxy) >= sqrt( pxx*pyy ) ||
         (isnan(pxx)||isinf(pxx))    ||
         (isnan(pxy)||isinf(pxy))    ||
         (isnan(pyy)||isinf(pyy))    )
    {
        std::stringstream ss;
        ss << "paintCovarianceEllipse(): covariance matrix not PD: pxx,pxy,pyy = "<<pxx<<","<<pxy<<","<<pyy;
//         ss << endl << "fabs(pxy)-sqrt(pxx*pyy): " << fabs(pxy)<< "-" <<sqrt(pxx*pyy)<<" = "<< fabs(pxy)-sqrt(pxx*pyy);
//         ss << endl << "fabs(pxy) > sqrt( pxx*pyy ): " << (fabs(pxy) > sqrt( pxx*pyy ));
//         ss << endl << "isnan||isinf(pxx): " << (isnan(pxx)||isinf(pxx));
//         ss << endl << "isnan||isinf(pxy): " << (isnan(pxy)||isinf(pxy));
//         ss << endl << "isnan||isinf(pyy): " << (isnan(pyy)||isinf(pyy));
        throw orcaqgui::Exception( ss.str() );
    }

    // Work out the details of the uncertainty ellipse
    double a, b, psi;
    Cov2d cov( pxx, pxy, pyy );
    cov.ellipse( a, b, psi );

    QPointF covPoint(a,b);
    float psiDeg = (int)ceil( psi *180/M_PI );            // rot. angle into princ. axes

    // Paint uncertainty ellipse
    p->save();
    {
        p->rotate( psiDeg );
        p->setPen( QPen( colour, THIN_LINE_THICKNESS ) );
        p->setBrush( Qt::NoBrush );
        p->drawEllipse( QRectF( -N_SIGMA*a,
                                -N_SIGMA*b,
                                2*N_SIGMA*a,
                                2*N_SIGMA*b ) );
    }
    p->restore();
}


QColor
getTransparentVersion( const QColor &c, float transparencyMultiplier )
{
    // Lower alpha gives a more transparent version.
    int alpha = (int) (c.alpha()*transparencyMultiplier);
    return QColor( c.red(),
                   c.green(),
                   c.blue(),
                   alpha );
}

void paintWaypoint( QPainter *p,
                    QColor &fillColor,
                    QColor &drawColor,
                    int   targetHeading,
                    float distanceTolerance,
                    int   headingTolerance )
{
    p -> setPen( QPen( drawColor, 0.05 ) );
    p -> drawEllipse( QRectF( -distanceTolerance,
                      -distanceTolerance,
                      2*distanceTolerance,
                      2*distanceTolerance) );       // draws circle as tolerances
    p -> setBrush( fillColor );
//    const float diam = 0.05;
//    p -> drawEllipse( QRectF( -diam,-diam, 2*diam+1, 2*diam+1) );                 // draws small circle at waypoint
    int a = -(targetHeading-headingTolerance);
    int alen = -2*headingTolerance;
    // !!!!!!!!!! WARNING!!!!!!!!!!!!!!!!!!!!
    // Qt documentation is wrong: positive is clockwise and negative is counterclockwise
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    p -> drawPie( QRectF( -distanceTolerance,
                  -distanceTolerance,
                  2*distanceTolerance,
                  2*distanceTolerance ),
    a,
    alen );
}

} // namespace
