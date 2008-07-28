/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <QPainter>
#include <QDialog>
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguielementutil/paintutils.h>
#include <orcaqgui2dfactory/waypointdialog.h>
#include <orcaqgui2dfactory/pathutils.h>

#include "pathdesignscreen.h"

using namespace std;

namespace orcaqgui2d {
    
namespace {    
    
    // normalises heading to 0<angle<360*16 for Gui painting
    int guiNormalise( int heading )
    {
        if (heading < 0) return (heading + 360*QT_ANGLE_MULTIPLIER);
        if (heading > 360*QT_ANGLE_MULTIPLIER) return (heading - 360*QT_ANGLE_MULTIPLIER);
        
        return heading;
    }
}

    
PathDesignScreen::PathDesignScreen( GuiPath                             &guiPath,
                                    WaypointSettings                    *wpSettings,
                                    hydroqguielementutil::IHumanManager &humanManager )
    : guiPath_(guiPath),
      wpSettings_(wpSettings),
      humanManager_(humanManager),      
      waypointInFocus_(-1)
{       
}


void PathDesignScreen::setWaypointFocus( int waypointId )
{
    waypointInFocus_ = waypointId;    
}

void
PathDesignScreen::resizeData( int index )
{
    guiPath_.resize( index );
}


void PathDesignScreen::paint( QPainter *p )
{
    QMatrix wm = p->matrix(); 
    wmInv_ = wm.inverted();
    
    if ( guiPath_.isEmpty() ) return;
    
    const float PATH_WIDTH = 0.05;
    
    QColor fillColor;
    QColor drawColor;
    
    if (useTransparency_) {
        fillColor = hydroqguielementutil::getTransparentVersion(Qt::green);
    } else {
        fillColor=Qt::green;
    }
    
    for ( int i=0; i<guiPath_.size(); i++)
    {
        p->save();
        p->translate( guiPath_[i].position.x(), guiPath_[i].position.y() );    // move to point

        if (i==waypointInFocus_) {
            drawColor = Qt::black;
        } else {
            drawColor = fillColor;
        }
        
        hydroqguielementutil::paintWaypoint(  p, 
                                        fillColor,
                                        drawColor, 
                                        guiPath_[i].heading,
                                        guiPath_[i].distanceTolerance,
                                        guiPath_[i].headingTolerance );

        p->restore();
    }
    
    // ===== draw the waypoint in focus again, to be able to see the edge =======
    if ( (waypointInFocus_!=-1) && (guiPath_.size()>0) )
    {
        p->save();

        p->translate( guiPath_[waypointInFocus_].position.x(), guiPath_[waypointInFocus_].position.y() );    // move to point
        drawColor = Qt::black;
        hydroqguielementutil::paintWaypoint(  p, 
                                        fillColor,
                                        drawColor, 
                                        guiPath_[waypointInFocus_].heading,
                                        guiPath_[waypointInFocus_].distanceTolerance,
                                        guiPath_[waypointInFocus_].headingTolerance );
        p->restore();
    }
    
    // draw connections between them
    if ( guiPath_.size()>1 )
    {
        p->setPen( QPen( fillColor, PATH_WIDTH ) );
        p->setBrush ( Qt::NoBrush );

        for ( int i=1; i<guiPath_.size(); ++i)
        {
            p->drawLine(guiPath_[i].position, guiPath_[i-1].position);
        }
    }
}

void PathDesignScreen::updateWpSettings( WaypointSettings* wpSettings )
{
    wpSettings_ = wpSettings;   
}

void PathDesignScreen::processPressEvent( QMouseEvent* e)
{
    mouseDownPnt_ = e->pos(); 
}

void PathDesignScreen::processReleaseEvent( QMouseEvent* e)
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

void PathDesignScreen::addWaypoint (QPointF wp)
{      
    assert(wpSettings_!=NULL);
    
    int numWaypoints = guiPath_.size();
    
    GuiWaypoint gwp;
    gwp.position = wp;
    guiPath_.push_back( gwp );
     
    if (numWaypoints==0)
    {   // first waypoint is special
        guiPath_[0].heading = 0;
        guiPath_[0].headingTolerance = 180*QT_ANGLE_MULTIPLIER;
        guiPath_[0].timeTarget = 0.0;
    }
    else 
    {
        // times: dependent on spacing mode
        float timeDelta;
        if( wpSettings_->spacingProperty=="Time" ) {
            timeDelta = wpSettings_->spacingValue;
        } else {
            timeDelta = straightLineDist( guiPath_[numWaypoints].position-guiPath_[numWaypoints-1].position ) / (wpSettings_->spacingValue);
        }
        
        guiPath_[numWaypoints].timeTarget = timeDelta + guiPath_[numWaypoints-1].timeTarget;
        
        // heading: aligned with the line between the two last waypoints
        QPointF diff = guiPath_[numWaypoints].position - guiPath_[numWaypoints-1].position;
        int tmpHeading =(int)floor( RAD2DEG(atan2(diff.y(),diff.x())) ) * QT_ANGLE_MULTIPLIER;
        guiPath_[numWaypoints].heading = guiNormalise(tmpHeading);
        
        guiPath_[numWaypoints].headingTolerance = wpSettings_->headingTolerance*QT_ANGLE_MULTIPLIER;
        // correct previous headings to the direction we're going
        if (numWaypoints>1)
        {
            guiPath_[numWaypoints-1].heading = tmpHeading;
        }
    }
    
    guiPath_[numWaypoints].distanceTolerance = wpSettings_->distanceTolerance;
    guiPath_[numWaypoints].maxSpeed = wpSettings_->maxApproachSpeed;
    guiPath_[numWaypoints].maxTurnrate = wpSettings_->maxApproachTurnrate;
}

void PathDesignScreen::removeWaypoint( QPointF p1 )
{
    if (guiPath_.isEmpty() ) return;
    const double VICINITY = 1.0; // in meters
    
    // check if click is near the latest waypoint
    int lastIndex = guiPath_.size() - 1;
    QPointF p2;
    p2 = guiPath_[lastIndex].position;
    double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
    if (dist<VICINITY)
    {
        // delete the last element
        resizeData( lastIndex );
    }
    
    // if the waypoint was in focus before, reset to no focus
    if (waypointInFocus_==lastIndex)
        waypointInFocus_=-1;
    
}

void PathDesignScreen::changeWpParameters( QPointF p1 )
{
    if ( guiPath_.isEmpty() ) return;    
    const double VICINITY = 1.0; // in meters
    
    // check if click is near a waypoint
    for (int i=0; i<guiPath_.size(); i++)
    {
        QPointF p2;
        p2 = guiPath_[i].position;
        double dist = sqrt ( (p1.x()-p2.x()) * (p1.x()-p2.x()) + (p1.y()-p2.y()) * (p1.y()-p2.y()) );
        
        if (dist<VICINITY)
        {
            QDialog *myDialog = new QDialog;
            Ui::WaypointDialog ui;
            ui.setupUi(myDialog);
            
            ui.xSpin->setValue( guiPath_[i].position.x() );
            ui.ySpin->setValue( guiPath_[i].position.y() );
            ui.headingSpin->setValue( guiPath_[i].heading/QT_ANGLE_MULTIPLIER );
            ui.timeSpin->setValue( guiPath_[i].timeTarget );
            ui.distanceTolSpin->setValue( guiPath_[i].distanceTolerance );
            ui.headingTolSpin->setValue( guiPath_[i].headingTolerance/QT_ANGLE_MULTIPLIER );
            ui.maxSpeedSpin->setValue( guiPath_[i].maxSpeed );
            ui.maxTurnrateSpin->setValue( guiPath_[i].maxTurnrate );
            
            int ret = myDialog->exec();
            if (ret==QDialog::Rejected) return;
            
            guiPath_[i].position.setX( ui.xSpin->value() );
            guiPath_[i].position.setY( ui.ySpin->value() );
            guiPath_[i].heading = ui.headingSpin->value()*QT_ANGLE_MULTIPLIER;
            guiPath_[i].timeTarget = ui.timeSpin->value();
            guiPath_[i].distanceTolerance = ui.distanceTolSpin->value();
            guiPath_[i].headingTolerance = ui.headingTolSpin->value()*QT_ANGLE_MULTIPLIER;
            guiPath_[i].maxSpeed = ui.maxSpeedSpin->value();
            guiPath_[i].maxTurnrate = ui.maxTurnrateSpin->value();       
        }
    }
}

float
PathDesignScreen::secondsToCompleteLoop() const
{
    float timeToCompleteLoop;
    
    if( wpSettings_->spacingProperty=="Time" ) 
    {
        timeToCompleteLoop = wpSettings_->spacingValue;
    } 
    else 
    {
        timeToCompleteLoop = straightLineDist( guiPath_.last().position - guiPath_.first().position ) / (wpSettings_->spacingValue);
    }
    return timeToCompleteLoop;
}
    
}


