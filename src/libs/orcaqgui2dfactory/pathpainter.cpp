/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <qpainter.h>
#include <qstring.h>

#include <QFile>
#include <QTextStream>

#include <orcaice/orcaice.h>
#include <orcalogfactory/logstringutils.h>

#include <orcaqgui2d/paintutils.h>

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

PathPainter::~PathPainter()
{
}

void PathPainter::initialize(bool displayWaypoints, bool displayPastWaypoints, bool displayOlympicMarker, bool useTransparency)
{
    displayWaypoints_ = displayWaypoints;
    displayPastWaypoints_ = displayPastWaypoints;
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
    
    if ( z != Z_PATH ) return;

    QColor fillColor;
    QColor drawColor;
    QColor futureWpColor;
    
    if (inFocus_) {
        futureWpColor = color_;
    } else {
        futureWpColor = Qt::gray;
    }
      
    QColor inactiveWpColor;
    QColor pastWpColor;
    QColor currentWpColor;
    
    if (useTransparency_) {
        inactiveWpColor = getTransparentVersion(futureWpColor);
        pastWpColor = getTransparentVersion(futureWpColor,0.25);
        currentWpColor = getTransparentVersion(futureWpColor,0.75);
    } else {
        inactiveWpColor = futureWpColor;
        pastWpColor = futureWpColor;
        currentWpColor = futureWpColor;
    }
    
    for ( int i=0; i < guiPath_.size(); i++)
    {
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
            else
            {
                fillColor = futureWpColor;
                drawColor = futureWpColor;
            }
        }
 
        painter->save();
        painter->translate( guiPath_[i].position.x(), guiPath_[i].position.y() );    // move to point
        paintWaypoint( painter, 
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
        drawColor = Qt::black;
        paintWaypoint( painter, 
                        currentWpColor,
                        drawColor, 
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
        
        for ( int i=startI; i<guiPath_.size(); ++i)
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
    paintWaypoint( painter, c, c, 0, 0.3, 360*16 );
    
    // drawing velocity values as text
    painter->setFont( QFont("Helvetica [Cronyx]", 12) );
    const double lineSpacing = painter->fontMetrics().lineSpacing();
    const double offset = 0.4;
    QMatrix m = painter->matrix();                       // this is m2win matrix
    QPointF labelPos = QPointF(offset,offset) * m;       // x-label position in window cs
    painter->setMatrix( QMatrix() );
    
    painter->drawText( labelPos, "speed:" + QString::number( velocity, 'f', 2 ) + " m/s" );
    labelPos.setY( labelPos.y() + lineSpacing );
    painter->drawText( labelPos, "maxSpeed: " + QString::number( guiPath_[wpI].maxSpeed ) + " m/s" );
    
    painter->restore();
}


void PathPainter::savePath( const QString fileName, hydroqgui::IHumanManager *humanManager ) const
{
    if (guiPath_.size()==0)
    {
        humanManager->showDialogMsg(hydroqgui::IHumanManager::Warning, "Path has no waypoints!");
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager->showDialogMsg(hydroqgui::IHumanManager::Error, "Cannot create file " + fileName );
        return;
    }
    
    // convert gui path to an orca path
    orca::Path2d orcaPath;
    guiPathToOrcaPath( guiPath_, orcaPath );
    
    // save to file
    QTextStream out(&file);
    out << QString(orcalogfactory::toLogString( orcaPath ).c_str());
    file.close();
}

}


