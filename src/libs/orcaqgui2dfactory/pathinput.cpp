/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaqgui2d/paintutils.h>

#include <orcaqgui2dfactory/pathinput.h>
#include <orcaqgui2dfactory/waypointdialog.h>
#include <orcaobj/orcaobj.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <orcaice/orcaice.h>

#include <QPainter>
#include <QString>
#include <QMouseEvent>
#include <QDialog>
#include <QFile>
#include <QTextStream>

using namespace std;

namespace orcaqgui {

PathInput::PathInput( WaypointSettings *wpSettings )
    : wpSettings_(wpSettings)
{
}

void
PathInput::resizeData( int index )
{
    waypoints_.resize( index );
    headings_.resize( index );
    distTolerances_.resize( index );
    headingTolerances_.resize( index );
    maxSpeeds_.resize( index );
    maxTurnrates_.resize( index );
}


void PathInput::paint( QPainter *p )
{
    QMatrix wm = p->matrix(); 
    wmInv_ = wm.inverted();
    
    if ( waypoints_.isEmpty() ) return;
    
    const float PATH_WIDTH = 0.05;
    QColor color(getTransparentVersion(Qt::green));
    
    for ( int i=0; i<waypoints_.size(); i++)
    {
        p->save();
        p->translate( waypoints_[i].x(), waypoints_[i].y() );    // move to point

        paintWaypoint( p, 
                       color,
                       color, 
                       headings_[i],
                       distTolerances_[i], 
                       headingTolerances_[i] );

        p->restore();
    }
    
    // draw connections between them
    if ( waypoints_.size()>1 )
    {
        p->setPen( QPen( color, PATH_WIDTH ) );
        p->setBrush ( Qt::NoBrush );

        for ( int i=1; i<waypoints_.size(); ++i)
        {
            p->drawLine(waypoints_[i],waypoints_[i-1]);
        }
    }
}

void PathInput::updateWpSettings( WaypointSettings* wpSettings )
{
    wpSettings_ = wpSettings;   
}

void PathInput::processPressEvent( QMouseEvent* e)
{
    mouseDownPnt_ = e->pos();
    mouseMovePnt_ = mouseDownPnt_;  // initialisation of mouseMovePnt_ to same position   
}

void PathInput::processReleaseEvent( QMouseEvent* e)
{
    mouseUpPnt_ = e->pos();
    int button =  e->button();

    QPointF waypoint = wmInv_.map( mouseUpPnt_ );
    //cout << "mouseUpPnt_,waypoint: (" << mouseUpPnt_.x() << "," << mouseUpPnt_.y() <<")  (" << waypoint.x() << ","<< waypoint.y() << ")" << endl;
    
    if (button == Qt::LeftButton) 
    {         
        addWaypoint( waypoint );
    }
    else if (button == Qt::RightButton)
    {      
        removeWaypoint( waypoint );
    }
    else if (button == Qt::MidButton)
    {
        changeWpParameters( waypoint );    
    }
}

void PathInput::processDoubleClickEvent( QMouseEvent* e)
{
}

void PathInput::processMoveEvent( QMouseEvent* e)
{
}

void PathInput::addWaypoint (QPointF wp)
{      
    assert(wpSettings_!=NULL);
    
    int numWaypoints = waypoints_.size();
    
    waypoints_.append( wp );
     
    // by default, the heading is aligned with the line between the two last waypoints
    if (numWaypoints==0)
    {   // first waypoint is special
        headings_.append( 0 );
        headingTolerances_.append( 180*16 );
    }
    else 
    {
        QPointF diff = waypoints_[numWaypoints] - waypoints_[numWaypoints - 1];
        int tmpHeading =(int)floor( atan2(diff.y(),diff.x() )/M_PI*180.0 );
        if (tmpHeading < 0) tmpHeading = tmpHeading + 360;
        headings_.append( tmpHeading*16 );
        headingTolerances_.append( wpSettings_->headingTolerance*16 );
        // correct past headings to the direction we're going
        if (numWaypoints>1)
        {
            headings_[numWaypoints-1] = tmpHeading*16;
        }
    }
    
    times_.append( wpSettings_->timePeriod * (numWaypoints+1) );
    distTolerances_.append( wpSettings_->distanceTolerance );
    maxSpeeds_.append( wpSettings_->maxApproachSpeed );
    maxTurnrates_.append( wpSettings_->maxApproachTurnrate );
}

void PathInput::removeWaypoint( QPointF p1 )
{
    if ( waypoints_.isEmpty() ) return;   
    const double VICINITY = 1.0; // in meters
    
    // check if click is near the latest waypoint
    int lastIndex = waypoints_.size() - 1;
    QPointF p2;
    p2 = waypoints_[lastIndex];
    double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
    if (dist<VICINITY)
    {
        // delete the last element
        resizeData( lastIndex );
    }
}

void PathInput::changeWpParameters( QPointF p1 )
{

    if ( waypoints_.isEmpty() ) return;    
    const double VICINITY = 1.0; // in meters
    
    // check if click is near a waypoint
    for (int i=0; i<waypoints_.size(); i++)
    {
        QPointF p2;
        p2 = waypoints_[i];
        double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
        
        if (dist<VICINITY)
        {
            QDialog *myDialog = new QDialog;
            Ui::WaypointDialog ui;
            ui.setupUi(myDialog);
            
            ui.xSpin->setValue( waypoints_[i].x() );
            ui.ySpin->setValue( waypoints_[i].y() );
            ui.headingSpin->setValue( headings_[i]/16 );
            ui.timeSpin->setValue( times_[i] );
            ui.distanceTolSpin->setValue( distTolerances_[i] );
            ui.headingTolSpin->setValue( headingTolerances_[i]/16 );
            ui.maxSpeedSpin->setValue( maxSpeeds_[i] );
            ui.maxTurnrateSpin->setValue( maxTurnrates_[i] );
            
            int ret = myDialog->exec();
            if (ret==QDialog::Rejected) return;
            
            waypoints_[i].setX( ui.xSpin->value() );
            waypoints_[i].setY( ui.ySpin->value() );
            headings_[i] = ui.headingSpin->value()*16;
            times_[i] = ui.timeSpin->value();
            distTolerances_[i] = ui.distanceTolSpin->value();
            headingTolerances_[i] = ui.headingTolSpin->value()*16;
            maxSpeeds_[i] = ui.maxSpeedSpin->value();
            maxTurnrates_[i] = ui.maxTurnrateSpin->value();
            
        }
    }

}

PathFollowerInput::PathFollowerInput( WaypointSettings *wpSettings )
    : PathInput(wpSettings)
{
}

orca::PathFollower2dDataPtr 
PathFollowerInput::getPath() const
{
    int size = waypoints_.size();
    
    orca::PathFollower2dDataPtr pathData = new orca::PathFollower2dData;
    pathData->path.resize( size );
    
    for (int i=0; i<size; i++)
    {
        float heading = headings_[i]/16;
        if (heading>180.0) {
            heading = heading - 360.0;
        }
        float headingTolerance = headingTolerances_[i]/16;
        if (headingTolerance>180.0) {
            headingTolerance = headingTolerance - 360.0;
        }

        pathData->path[i].target.p.x = waypoints_[i].x();
        pathData->path[i].target.p.y = waypoints_[i].y();
        pathData->path[i].target.o = heading/180.0 * M_PI;
        pathData->path[i].distanceTolerance = distTolerances_[i];
        pathData->path[i].headingTolerance = (float)headingTolerance/180.0*M_PI;      
        pathData->path[i].timeTarget = orcaice::toOrcaTime( times_[i] );
            
        pathData->path[i].maxApproachSpeed = maxSpeeds_[i];
        pathData->path[i].maxApproachTurnrate = (float)maxTurnrates_[i]/180.0*M_PI;
    }

    return pathData;
}

orca::PathPlanner2dTaskPtr
PathPlannerInput::getTask() const
{
    int size = waypoints_.size();
        
    orca::PathPlanner2dTaskPtr task = new orca::PathPlanner2dTask;
    task->coarsePath.resize( size );
        
    for (int i=0; i<size; i++)
    {
        float heading = headings_[i]/16;
        if (heading>180.0) {
            heading = heading - 360.0;
        }
        float headingTolerance = headingTolerances_[i]/16;
        if (headingTolerance>180.0) {
            headingTolerance = headingTolerance - 360.0;
        }
        orca::Waypoint2d wp;
        wp.target.p.x = waypoints_[i].x();
        wp.target.p.y = waypoints_[i].y();
        wp.target.o = heading/180.0 * M_PI;
        wp.distanceTolerance = distTolerances_[i];
        wp.headingTolerance = (float)headingTolerance/180.0*M_PI;      
        wp.timeTarget = orcaice::toOrcaTime( times_[i] );
        wp.maxApproachSpeed = maxSpeeds_[i];
        wp.maxApproachTurnrate = (float)maxTurnrates_[i]/180.0*M_PI;
            
        task->coarsePath[i] = wp;
    }

    return task;
}

void PathFollowerInput::loadDataFromFile( QString* fileName )
{    
    resizeData(0);
    
    QFile file(*fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        waypoints_.append( QPointF(line.section(' ',0,0).toFloat(),line.section(' ',1,1).toFloat()) );
        headings_.append( line.section(' ',2,2).toInt() );
        times_.append( line.section(' ',3,3).toFloat() );
        distTolerances_.append( line.section(' ',4,4).toFloat() );
        headingTolerances_.append( line.section(' ',5,5).toInt() );
        maxSpeeds_.append( line.section(' ',6,6).toFloat() );
        maxTurnrates_.append( line.section(' ',7,7).toInt() );
    }
}


//////////////////////////////////////////////////////////////////////

WaypointSettings
readWaypointSettings( const orcaice::Context  &context )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.";

    float timePeriod = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"TimePeriod", 5.0 );
    float distanceTolerance = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"DistanceTolerance", 1.0 );
    int headingTolerance = orcaice::getPropertyAsIntWithDefault( prop, prefix+"HeadingTolerance", 45 );
    float maxApproachSpeed = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxApproachSpeed", 2e6 );
    int maxApproachTurnrate = orcaice::getPropertyAsIntWithDefault( prop, prefix+"MaxApproachTurnRate", 2000000 );
    WaypointSettings wpSettings(timePeriod, distanceTolerance, headingTolerance, maxApproachSpeed, maxApproachTurnrate);
    return wpSettings;
}

bool readActivateImmediately( orcaice::Context &context )
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag() + ".Config.";
    return orcaice::getPropertyAsIntWithDefault( prop, prefix+"ActivatePathImmediately", 1 );
}


}


