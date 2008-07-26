#include <orcaqgui2dfactory/pathfollowerhi.h>
#include "pathfollowerinput.h"

namespace orcaqgui2d {

PathFollowerInput::PathFollowerInput ( PathFollowerHI                      &pathFollowerHI,
                                       WaypointSettings                    *wpSettings,
                                       hydroqguielementutil::IHumanManager &humanManager,
                                       const QString                       &lastSavedPathFile )
    : pathFollowerHI_(pathFollowerHI)
{
    guiPath_.reset( new GuiPath() );
    pathDesignScreen_.reset( new PathDesignScreen( *guiPath_.get(), wpSettings, humanManager ) );
    pathDesignWidget_.reset( new PathDesignWidget( this, *guiPath_.get() ) );
    pathFileHandler_.reset( new PathFileHandler( humanManager, lastSavedPathFile ) );
}

void 
PathFollowerInput::paint( QPainter *painter )
{
    pathDesignScreen_->paint( painter );
}

void
PathFollowerInput::setUseTransparency( bool useTransparency ) 
{
    pathDesignScreen_->setUseTransparency( useTransparency );
}

void 
PathFollowerInput::processPressEvent( QMouseEvent* e)
{
    pathDesignScreen_->processPressEvent( e );
}

void PathFollowerInput::processReleaseEvent( QMouseEvent* e ) 
{
    pathDesignScreen_->processReleaseEvent( e );
    
    if ( pathDesignWidget_->isHidden() ) 
        pathDesignWidget_->show();
    pathDesignWidget_->refreshTable();
}

void 
PathFollowerInput::updateWpSettings( WaypointSettings* wpSettings )
{
    pathDesignScreen_->updateWpSettings( wpSettings );
}

void 
PathFollowerInput::setWaypointFocus( int waypointId )
{
    pathDesignScreen_->setWaypointFocus( waypointId );
}

bool
PathFollowerInput::getPath( orca::PathFollower2dData &pathData ) const
{    
    int size = pathDesignWidget_->numberOfLoops() * guiPath_->size();
    //cout << "DEBUG(pathinput.cpp): getPath: size of waypoints is " << size << endl;
    if (size==0) return false;
    
    const float timeOffset = guiPath_->last().timeTarget + pathDesignScreen_->secondsToCompleteLoop();
    guiPathToOrcaPath( *guiPath_.get(), pathData.path, pathDesignWidget_->numberOfLoops(), timeOffset );
    
    return true;
}

void 
PathFollowerInput::savePath( const QString &filename )
{
    int numLoops = pathDesignWidget_->numberOfLoops();
    
    float timeOffset = 0.0;
    if (numLoops > 1)
        timeOffset = guiPath_->last().timeTarget + pathDesignScreen_->secondsToCompleteLoop();
  
    pathFileHandler_->savePath( filename, *guiPath_.get(), numLoops, timeOffset );  
}

void
PathFollowerInput::loadPath( const QString &filename ) 
{  
    pathFileHandler_->loadPath( filename, *guiPath_.get() );
    pathDesignWidget_->refreshTable(); 
}

void 
PathFollowerInput::loadPreviousPath()
{
    pathFileHandler_->loadPreviousPath( *guiPath_.get() );
    pathDesignWidget_->refreshTable(); 
}

void
PathFollowerInput::sendPath() 
{ 
    pathFollowerHI_.send(); 
}

void 
PathFollowerInput::cancelPath() 
{ 
    pathFollowerHI_.cancel(); 
}

}
