/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <gbxutilacfr/mathdefs.h>
#include <hydroqguielementutil/paintutils.h>
#include <orcaqgui2dfactory/pathconversionutil.h>

#include "pathfollowerpainter.h"

using namespace std;

namespace orcaqgui2d {

class WpColors
{
    public:
    
    WpColors( QColor &baseColor, bool useTransparency )
    {
        if (useTransparency) {
            finishedPathColor_ = hydroqguielementutil::getTransparentVersion(baseColor,0.1);
            notActivatedPathColor_ = hydroqguielementutil::getTransparentVersion(baseColor,0.2);
            pastWpColor_ = hydroqguielementutil::getTransparentVersion(baseColor,0.4);
            currentWpColor_ = hydroqguielementutil::getTransparentVersion(baseColor,0.7);
            futureWpColor_ = hydroqguielementutil::getTransparentVersion(baseColor,0.5);
            lineWpColor_ = hydroqguielementutil::getTransparentVersion(baseColor,0.7 );
        } else {
            finishedPathColor_ = baseColor;
            notActivatedPathColor_ = baseColor;
            pastWpColor_ = baseColor;
            currentWpColor_ = baseColor;
            futureWpColor_ = baseColor;
            lineWpColor_ = baseColor;
        }
        
    }

    QColor &currentWpColor()
        { return currentWpColor_; }

    QColor &lineWpColor()
        { return lineWpColor_; }

    void lookupColors( QColor &fillColor,
                       QColor &drawColor,
                       int currentWp,
                       int wpIndex,
                       orca::PathActivationEnum activationState )
    {

        if ( activationState==orca::FinishedPath )
        {
            fillColor = finishedPathColor_;
            drawColor = finishedPathColor_;
        }
        else if ( activationState==orca::PathLoadedButNotActivated )
        {
           fillColor = notActivatedPathColor_;
           drawColor = notActivatedPathColor_;
        }
        else
        {
            if ( currentWp == wpIndex )
            {
                fillColor = currentWpColor_;
                drawColor = Qt::black;
            }
            else if ( currentWp < wpIndex )
            {
                fillColor = pastWpColor_;
                drawColor = pastWpColor_;
            }
            else // currentWp > wpIndex
            {
                fillColor = futureWpColor_;
                drawColor = futureWpColor_;
            }
        }
    }

    private:
        QColor finishedPathColor_;
        QColor notActivatedPathColor_;
        QColor pastWpColor_;
        QColor currentWpColor_;
        QColor futureWpColor_;
        QColor lineWpColor_;
};

// ==========================================================================================

PathFollowerPainter::PathFollowerPainter()
    : wpIndex_(-1),
      activationState_(orca::NoPathLoaded),
      color_(Qt::blue),
      inFocus_(true),
      relativeStartTime_(NAN)
{
}

void PathFollowerPainter::setData( const orca::PathFollower2dData& path )
{
    // this fct is called when the follower just received a new path
    orcaPathToGuiPath( path.path, guiPath_ );
    activationState_ = orca::PathLoadedButNotActivated;
    wpIndex_ = -1;
}

void PathFollowerPainter::setRelativeStartTime( double relativeStartTime )
{
    relativeStartTime_ = relativeStartTime;
    
    // Start a timer to keep track of the time since activation internally.
    // Whenever this function is called again, we get an updated relativeStartTime 
    // which will reset the internal clock.
    olympicMarkerTimer_.restart();
}

void PathFollowerPainter::setState( const orca::PathFollower2dState& state )
{
    wpIndex_ = state.waypointIndex;
    activationState_ = state.pathActivation;
    setRelativeStartTime( state.secondsSinceActivation );
}

bool
PathFollowerPainter::olympicMarkerPosAndSpeed( float &x, float &y, double &velocity )
{
    double relStartTimeLocal = relativeStartTime_ + olympicMarkerTimer_.elapsedSec();

    int wpI = -1;
    // find the waypoint we should be going towards according to the plan
    for (unsigned int i=0; i<guiPath_.size(); i++)
    {
        if (relStartTimeLocal <= guiPath_[i].timeTarget) {
            wpI = i;
            break;
        }
    }

    if (wpI == -1) {
        // we're at the goal
        return true;
    }

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
        float ratio = (relStartTimeLocal-guiPath_[wpI-1].timeTarget) /
                      (guiPath_[wpI].timeTarget - guiPath_[wpI-1].timeTarget);

        // compute position of sliding point
        QPointF diffPoints = guiPath_[wpI].position - guiPath_[wpI-1].position;
        x = guiPath_[wpI-1].position.x() + ratio * diffPoints.x();
        y = guiPath_[wpI-1].position.y() + ratio * diffPoints.y();
        velocity = sqrt( diffPoints.x()*diffPoints.x() + diffPoints.y()*diffPoints.y() ) /
                    (guiPath_[wpI].timeTarget - guiPath_[wpI-1].timeTarget);
    }

    return false;
}


void
PathFollowerPainter::drawOlympicMarker( QPainter *painter, float x, float y, double velocity )
{
    if (!config_.displayOlympicMarker) return;
    if (relativeStartTime_==NAN) return;

    painter->save();
    painter->translate( x, y );
    QColor c = Qt::blue;
    hydroqguielementutil::paintWaypoint( painter, c, c, 0, 0.3, 360*16 );

    // drawing velocity values as text
    painter->setFont( QFont("Helvetica [Cronyx]", 12) );
    const double offset = 0.4;
    QMatrix m = painter->matrix();                       // this is m2win matrix
    QPointF labelPos = QPointF(offset,offset) * m;       // x-label position in window cs
    painter->setMatrix( QMatrix() );

    painter->drawText( labelPos, QString::number( velocity, 'f', 2 ) + " m/s" );

//    painter->drawText( labelPos, "speed:" + QString::number( velocity, 'f', 2 ) + " m/s" );
//    const double lineSpacing = painter->fontMetrics().lineSpacing();
//    labelPos.setY( labelPos.y() + lineSpacing );
//    painter->drawText( labelPos, "maxSpeed: " + QString::number( guiPath_[wpI].maxSpeed ) + " m/s" );

    painter->restore();
}

void PathFollowerPainter::paint( QPainter *painter, int z )
{
    if ( z != hydroqguielementutil::Z_PATH ) return;
    if ( !config_.displayWaypoints ) return;

    switch (activationState_)
    {
        case orca::NoPathLoaded :
            return;
        case orca::FinishedPath :
            if (!config_.displayPathWhenFinished) return;
        case orca::PathLoadedButNotActivated :
            if (!config_.displayPathWhenNotActivated) return;
        case orca::FollowingPath :
            break;
        default: assert( false && "Unknown activation state" );
    }

    // this guy handles the waypoint colors
    WpColors wpColors( color_, config_.useTransparency );

    // === drawing individual waypoints ===
    for ( int i=0; i < (int)guiPath_.size(); i++)
    {
        if (activationState_==orca::FollowingPath)
        {
            if (i<wpIndex_ && !config_.displayPastWaypoints ) continue;
            if (i>wpIndex_ && !config_.displayFutureWaypoints ) break;
        }

        QColor fillColor;
        QColor drawColor;
        wpColors.lookupColors( fillColor, drawColor, i, wpIndex_, activationState_ );
        hydroqguipath::drawWaypoint( painter, guiPath_, i, fillColor, drawColor );
    }

    // === draw the current target waypoint again, to be able to see the edge if we have loops ===
    if ( (activationState_==orca::FollowingPath) && (guiPath_.size()>0) )
        hydroqguipath::drawWaypoint( painter, guiPath_, wpIndex_, wpColors.currentWpColor(), Qt::black );

    // === draw connections between waypoints ===
    int startI = 0;
    if ( activationState_==orca::FollowingPath && !config_.displayPastWaypoints )
        startI = wpIndex_+1;
    int endI = guiPath_.size();
    if ( activationState_==orca::FollowingPath && !config_.displayFutureWaypoints )
        endI = MIN( wpIndex_+1, (int)guiPath_.size() );
    hydroqguipath::drawWpConnections( painter, guiPath_, wpColors.lineWpColor(), startI, endI );

    // === draw the olympic marker: shows where we should be according to the plan ===
    if (activationState_!=orca::FollowingPath) return;
    float x;
    float y;
    double velocity;
    bool atGoal = olympicMarkerPosAndSpeed( x, y, velocity );
    if (!atGoal)
        drawOlympicMarker( painter, x, y, velocity );
}

}


