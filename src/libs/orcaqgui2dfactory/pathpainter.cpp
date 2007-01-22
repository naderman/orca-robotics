/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <qpainter.h>
#include <qstring.h>

#include <QFile>
#include <QTextStream>

#include <orcaice/orcaice.h>

#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2d/paintutils.h>

#include "pathpainter.h"

using namespace orca;
using namespace std;

namespace orcaqgui {

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

void PathPainter::initialize(bool displayWaypoints, bool displayPastWaypoints, bool useTransparency)
{
    displayWaypoints_ = displayWaypoints;
    displayPastWaypoints_ = displayPastWaypoints;
    useTransparency_ = useTransparency;
}

void PathPainter::clear()
{
    waypoints_.resize(0);
}

void PathPainter::setData( const PathFollower2dData& path )
{
    Path2d path2d = path.path;
    setDataLocal( path2d );
}

void PathPainter::setData( const PathPlanner2dData& path )
{
    Path2d path2d = path.path;
    setDataLocal( path2d );
}

void PathPainter::setRelativeStartTime( double relativeStartTime )
{
    //cout << "TRACE(pathpainter.cpp): new relative time: " << relativeStartTime << endl;
    relativeStartTime_ = relativeStartTime;
}

void PathPainter::setDataLocal( Path2d & path )
{
    orca::Waypoint2d wayPoint;
    
    int numWaypoints = path.size();
    
    waypoints_.resize( numWaypoints );
    headings_.resize( numWaypoints );
    distTolerances_.resize( numWaypoints );
    headingTolerances_.resize( numWaypoints );  
    times_.resize( numWaypoints ); 
    maxSpeeds_.resize( numWaypoints ); 
    maxTurnrates_.resize( numWaypoints );   

    for ( int i=0; i < numWaypoints; i++ )
    {
        wayPoint = path[i];

        waypoints_[i] = QPointF( wayPoint.target.p.x, wayPoint.target.p.y );
        const double factor = 16.0;     // factor for drawPie where angles are required as 1/16 deg 
        double heading;
        // conversion to 0->360*16
        if (wayPoint.target.o < 0.0)
        {
            heading = wayPoint.target.o + 2.0* M_PI;
        }
        else
        {
            heading = wayPoint.target.o;
        }
        headings_[i] = (int)floor( factor * (heading*180.0/M_PI) );
        distTolerances_[i] = wayPoint.distanceTolerance;
        headingTolerances_[i] = (int)floor( factor* (wayPoint.headingTolerance/M_PI*180.0));
        
        times_[i] = orcaice::timeAsDouble(wayPoint.timeTarget);
        maxSpeeds_[i] = wayPoint.maxApproachSpeed; 
        maxTurnrates_[i] = (int)RAD2DEG(wayPoint.maxApproachTurnrate);
    }
}

void PathPainter::setWpIndex( int index )
{
    wpIndex_ = index;
}

void PathPainter::paint( QPainter *painter, int z )
{
    if ( !displayWaypoints_ ) return;
    
    if ( z != orcaqgui::Z_PATH ) return;

    const float PATH_WIDTH = 0.05;

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
    
    for ( int i=0; i < waypoints_.size(); i++)
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
        painter->translate( waypoints_[i].x(), waypoints_[i].y() );    // move to point
        paintWaypoint( painter, 
                       fillColor,
                       drawColor, 
                       headings_[i],
                       distTolerances_[i], 
                       headingTolerances_[i] );
        painter->restore();
    }
    
    // ===== draw the waypoint in focus again, to be able to see the edge =======
    if ( (wpIndex_!=-1) && (waypoints_.size()>0) )
    {
        painter->save();

        painter->translate( waypoints_[wpIndex_].x(), waypoints_[wpIndex_].y() );    // move to point
        drawColor = Qt::black;
        paintWaypoint( painter, 
                        currentWpColor,
                        drawColor, 
                        headings_[wpIndex_],
                        distTolerances_[wpIndex_], 
                        headingTolerances_[wpIndex_] );
        painter->restore();
    }
    
    // ======== draw connections between waypoints =========
    if ( waypoints_.size()>1 )
    {
        painter->setPen( QPen( futureWpColor, PATH_WIDTH ) );
        painter->setBrush ( Qt::NoBrush );
        int startI = 0;
        if ( !displayPastWaypoints_ ) 
            startI = wpIndex_+1;
        assert( startI >= 0 );
        for ( int i=startI; i<waypoints_.size(); ++i)
        {
            if (i != 0) painter->drawLine(waypoints_[i],waypoints_[i-1]);
        }
    }
    
    // ====== draw a point where we should be according to the plan ========
    if (relativeStartTime_==NAN) return;
    
    int wpI = -1;
    // find the waypoint we should be going towards according to the plan
    for (int i=0; i<waypoints_.size(); i++)
    {
        if (relativeStartTime_ < times_[i]) {
            wpI = i;
            break;
        }
    }
    if (wpI == -1) {
        // we're at the goal
        return;
    }
    
    // ratio of how much we accomplished of the distance between the two current waypoints
    float ratio = (relativeStartTime_-times_[wpI-1])/(times_[wpI] - times_[wpI-1]);

    // compute position of sliding point
    QPointF diffPoints = waypoints_[wpI] - waypoints_[wpI-1];      
    float x = waypoints_[wpI-1].x() + ratio * diffPoints.x();
    float y = waypoints_[wpI-1].y() + ratio * diffPoints.y();
    
    painter->save();
    painter->translate( x, y );
    QColor c = Qt::blue;
    paintWaypoint( painter, c, c, 0, 0.3, 360*16 );
    painter->restore();
}


int PathPainter::savePath( const QString fileName, IHumanManager *humanManager ) const
{
    int size=waypoints_.size();
    
    if (size==0)
    {
        humanManager->showBoxMsg(Warning, "Path has no waypoints. File will be empty!");
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        humanManager->showBoxMsg(Error, "Cannot create file " + fileName );
        return -1;
    }

    QTextStream out(&file);
    for (int i=0; i<size; i++)
    {
        out << waypoints_[i].x() << " " << waypoints_[i].y() << " "
                << headings_[i] << " "
                << times_[i] << " "
                << distTolerances_[i] << " "
                << headingTolerances_[i]<< " "
                << maxSpeeds_[i]<< " "
                << maxTurnrates_[i]<< "\n";
    }
    
    file.close();
    return 0;
}

}


