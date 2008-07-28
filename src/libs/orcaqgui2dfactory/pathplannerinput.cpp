#include <orcaqgui2dfactory/pathplanneruserinteraction.h>
#include <orcaqgui2dfactory/pathconversionutil.h>
#include "pathplannerinput.h"

namespace orcaqgui2d {

PathPlannerInput::PathPlannerInput( PathPlannerUserInteraction          &pathPlannerUI,
                                    WaypointSettings                    *wpSettings,
                                    hydroqguielementutil::IHumanManager &humanManager )
    : pathPlannerUI_(pathPlannerUI)
{
    guiPath_.reset( new GuiPath() );
    
    pathDesignScreen_.reset( new PathDesignScreen( *guiPath_.get(), wpSettings, humanManager ) );
    pathDesignTableWidget_.reset( new PathDesignTableWidget( this, *guiPath_.get() ) );
    pathFileHandler_.reset( new PathFileHandler( humanManager ) );
}

void 
PathPlannerInput::paint( QPainter *painter )
{
    pathDesignScreen_->paint( painter );
}

void
PathPlannerInput::setUseTransparency( bool useTransparency ) 
{
    pathDesignScreen_->setUseTransparency( useTransparency );
}

void 
PathPlannerInput::processPressEvent( QMouseEvent* e)
{
    pathDesignScreen_->processPressEvent( e );
}

void PathPlannerInput::processReleaseEvent( QMouseEvent* e ) 
{
    pathDesignScreen_->processReleaseEvent( e );
    
    if ( pathDesignTableWidget_->isHidden() ) 
        pathDesignTableWidget_->show();
    pathDesignTableWidget_->refreshTable();
}

void 
PathPlannerInput::updateWpSettings( WaypointSettings* wpSettings )
{
    pathDesignScreen_->updateWpSettings( wpSettings );
}

void 
PathPlannerInput::setWaypointFocus( int waypointId )
{
    pathDesignScreen_->setWaypointFocus( waypointId );
}

orca::PathPlanner2dTask
PathPlannerInput::getTask() const
{       
    orca::PathPlanner2dTask task;
    guiPathToOrcaPath( *guiPath_.get(), task.coarsePath );
    return task;
}

void 
PathPlannerInput::savePath( const QString &filename )
{
    int numLoops = pathDesignTableWidget_->numberOfLoops();
    
    float timeOffset = 0.0;
    if (numLoops > 1)
        timeOffset = guiPath_->last().timeTarget + pathDesignScreen_->secondsToCompleteLoop();
  
    pathFileHandler_->savePath( filename, *guiPath_.get(), numLoops, timeOffset );  
}

void
PathPlannerInput::loadPath( const QString &filename ) 
{  
    pathFileHandler_->loadPath( filename, *guiPath_.get() );
    pathDesignTableWidget_->refreshTable(); 
}

void 
PathPlannerInput::loadPreviousPath()
{
    pathFileHandler_->loadPreviousPath( *guiPath_.get() );
    pathDesignTableWidget_->refreshTable(); 
}

void
PathPlannerInput::sendPath() 
{ 
    pathPlannerUI_.send(); 
}

void 
PathPlannerInput::cancelPath() 
{ 
    pathPlannerUI_.cancel(); 
}
            
            
}
