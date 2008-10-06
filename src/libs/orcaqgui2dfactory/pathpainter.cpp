/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <hydroqguielementutil/paintutils.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaqgui2dfactory/pathconversionutil.h>

#include "pathpainter.h"

using namespace std;

namespace orcaqgui2d {

PathPainter::PathPainter()
    : wpIndex_(-1),
      color_(Qt::blue),
      inFocus_(true),
      relativeStartTime_(NAN)
{
}

void PathPainter::initialize(bool displayWaypoints, bool displayPastWaypoints, bool displayFutureWaypoints, bool displayOlympicMarker, bool useTransparency)
{
    displayWaypoints_ = displayWaypoints;
    displayPastWaypoints_ = displayPastWaypoints;
    displayFutureWaypoints_ = displayFutureWaypoints;
    displayOlympicMarker_ = displayOlympicMarker;
    useTransparency_ = useTransparency;
}

void PathPainter::clear()
{
    guiPath_.resize(0);
}

void PathPainter::setData( const orca::PathFollower2dData& path )
{
    orcaPathToGuiPath( path.path, guiPath_ );
}

void PathPainter::setData( const orca::PathPlanner2dData& path )
{
    orcaPathToGuiPath( path.path, guiPath_ );
}

void PathPainter::setRelativeStartTime( double relativeStartTime )
{
    //cout << "TRACE(pathpainter.cpp): new relative time: " << relativeStartTime << endl;
    relativeStartTime_ = relativeStartTime;
}

void PathPainter::setWpIndex( int index )
{
    wpIndex_ = index;
}

void PathPainter::paint( QPainter *painter, int z )
{
    if ( !displayWaypoints_ ) return;
    
    if ( z != hydroqguielementutil::Z_PATH ) return;

    QColor baseColor;
    if (inFocus_) {
        baseColor = color_;
    } else {
        baseColor = Qt::gray;
    }
      
    QColor inactiveWpColor = baseColor;
    QColor pastWpColor = baseColor;
    QColor currentWpColor = baseColor;
    QColor futureWpColor = baseColor;
    
    if (useTransparency_) {
        inactiveWpColor = hydroqguielementutil::getTransparentVersion(baseColor);
        pastWpColor = hydroqguielementutil::getTransparentVersion(baseColor,0.25);
        currentWpColor = hydroqguielementutil::getTransparentVersion(baseColor,0.75);
        futureWpColor = hydroqguielementutil::getTransparentVersion(baseColor,0.5);
    }
    
    for ( int i=0; i < guiPath_.size(); i++)
    {
        QColor fillColor;
        QColor drawColor;
        if ( wpIndex_ == -1 )
        {
            fillColor = inactiveWpColor;
            drawColor = inactiveWpColor;
        }
        else
        {
            if ( i == wpIndex_ )
            {
                fillColor = currentWpColor;
                drawColor = Qt::black;
            }
            else if ( i < wpIndex_ )
            {
                if ( !displayPastWaypoints_ ) continue;
                fillColor = pastWpColor;
                drawColor = pastWpColor;
            }
            else // i > wpIndex
            {
                if ( !displayFutureWaypoints_ ) break;
                fillColor = futureWpColor;
                drawColor = futureWpColor;
            }
        }
 
        painter->save();
        painter->translate( guiPath_[i].position.x(), guiPath_[i].position.y() );    // move to point
        hydroqguielementutil::paintWaypoint(  painter, 
                                        fillColor,
                                        drawColor, 
                                        guiPath_[i].heading,
                                        guiPath_[i].distanceTolerance, 
                                        guiPath_[i].headingTolerance );
        painter->restore();
    }
    
    // ===== draw the waypoint in focus again, to be able to see the edge =======
    if ( (wpIndex_!=-1) && (guiPath_.size()>0) )
    {
        painter->save();

        painter->translate( guiPath_[wpIndex_].position.x(), guiPath_[wpIndex_].position.y() );    // move to point
        hydroqguielementutil::paintWaypoint(  painter, 
                                        currentWpColor,
                                        Qt::black, 
                                        guiPath_[wpIndex_].heading,
                                        guiPath_[wpIndex_].distanceTolerance, 
                                        guiPath_[wpIndex_].headingTolerance );
        painter->restore();
    }
    
    // ======== draw connections between waypoints =========
    if ( guiPath_.size()>1 )
    {
        QPen pen( futureWpColor, 0.1 );
//TODO: this works for Qt 4.3
//         pen.setCosmetic( true );
        painter->setPen( pen );
        painter->setBrush ( Qt::NoBrush );
        int startI = 0;
        if ( !displayPastWaypoints_ ) 
            startI = wpIndex_+1;
        assert( startI >= 0 );

        int endI = guiPath_.size();
        if ( !displayFutureWaypoints_ ) endI = wpIndex_;

        for ( int i=startI; i<endI; ++i)
        {
            if (i != 0) painter->drawLine(guiPath_[i].position,guiPath_[i-1].position);
        }
    }
    
    // ====== draw the olympic marker: shows where we should be according to the plan ========
    if (!displayOlympicMarker_) return;
    if (relativeStartTime_==NAN) return;
    
    int wpI = -1;
    // find the waypoint we should be going towards according to the plan
    for (int i=0; i<guiPath_.size(); i++)
    {
        if (relativeStartTime_ <= guiPath_[i].timeTarget) {
            wpI = i;
            break;
        }
    }
    
    if (wpI == -1) {
        // we're at the goal
        return;
    }
    
    float x;
    float y;
    double velocity;
    
    if (wpI == 0) 
    {
        // we going for the very first waypoint: paint marker on top of it
        x = guiPath_[0].position.x();
        y = guiPath_[0].position.y();
        velocity = 0.0;
    }
    else
    {    
        // ratio of how much we accomplished of the distance between the two current waypoints
        float ratio = (relativeStartTime_-guiPath_[wpI-1].timeTarget) /
                      (guiPath_[wpI].timeTarget - guiPath_[wpI-1].timeTarget);
    
        // compute position of sliding point
        QPointF diffPoints = guiPath_[wpI].position - guiPath_[wpI-1].position;      
        x = guiPath_[wpI-1].position.x() + ratio * diffPoints.x();
        y = guiPath_[wpI-1].position.y() + ratio * diffPoints.y();
        velocity = sqrt( diffPoints.x()*diffPoints.x() + diffPoints.y()*diffPoints.y() ) /
                    (guiPath_[wpI].timeTarget - guiPath_[wpI-1].timeTarget);
    }
    
    painter->save();
    painter->translate( x, y );
    QColor c = Qt::blue;
    hydroqguielementutil::paintWaypoint( painter, c, c, 0, 0.3, 360*16 );
    
    // drawing velocity values as text
    painter->setFont( QFont("Helvetica [Cronyx]", 12) );
    // const double lineSpacing = painter->fontMetrics().lineSpacing();
    const double offset = 0.4;
    QMatrix m = painter->matrix();                       // this is m2win matrix
    QPointF labelPos = QPointF(offset,offset) * m;       // x-label position in window cs
    painter->setMatrix( QMatrix() );
    
    painter->drawText( labelPos, QString::number( velocity, 'f', 2 ) + " m/s" );
//    painter->drawText( labelPos, "speed:" + QString::number( velocity, 'f', 2 ) + " m/s" );
//    labelPos.setY( labelPos.y() + lineSpacing );
//    painter->drawText( labelPos, "maxSpeed: " + QString::number( guiPath_[wpI].maxSpeed ) + " m/s" );
    
    painter->restore();
}

}


