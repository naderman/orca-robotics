#include <QFileDialog>
#include <orcaqgui2dfactory/pathfolloweruserinteraction.h>
#include <orcaqgui2dfactory/pathconversionutil.h>
#include "pathfollowerinput.h"

namespace orcaqgui2d {

PathFollowerInput::PathFollowerInput ( hydroqguipath::IPathUserInteraction *pathUI,
                                       hydroqguipath::WaypointSettings           *wpSettings,
                                       hydroqguielementutil::IHumanManager       &humanManager )
    : pathUI_(pathUI)
      
{
    guiPath_.reset( new hydroqguipath::GuiPath() );
    pathDesignScreen_.reset( new hydroqguipath::PathDesignScreen( *guiPath_.get(), wpSettings, humanManager ) );
    pathDesignTableWidget_.reset( new hydroqguipath::PathDesignTableWidget( this, *guiPath_.get() ) );
}

PathFollowerInput::~PathFollowerInput()
{
    std::cout << "PathFollowerInput: destructing itself" << std::endl;
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
    
    if ( pathDesignTableWidget_->isHidden() ) 
        pathDesignTableWidget_->show();
    pathDesignTableWidget_->refreshTable();
}

void 
PathFollowerInput::updateWpSettings( hydroqguipath::WaypointSettings* wpSettings )
{
    pathDesignScreen_->updateWpSettings( wpSettings );
}

void 
PathFollowerInput::setWaypointFocus( int waypointId )
{
    pathDesignScreen_->setWaypointFocus( waypointId );
}

void 
PathFollowerInput::getPath( hydroqguipath::GuiPath &guiPath, 
                            int &numLoops, 
                            float &timeOffset ) const
{
    guiPath = *guiPath_.get();
    
    numLoops = pathDesignTableWidget_->numberOfLoops();
    
    if (numLoops > 1)
        timeOffset = guiPath_->back().timeTarget + pathDesignScreen_->secondsToCompleteLoop();
    else
        timeOffset=0.0;
}


void
PathFollowerInput::savePath()
{
    int numLoops = pathDesignTableWidget_->numberOfLoops();
    
    float timeOffset = 0.0;
    if (numLoops > 1)
        timeOffset = guiPath_->back().timeTarget + pathDesignScreen_->secondsToCompleteLoop();
  
    pathUI_->saveUserPath( *guiPath_.get(), numLoops, timeOffset );
}


void
PathFollowerInput::loadPath() 
{  
    pathUI_->loadUserPath( *guiPath_.get() );
    pathDesignTableWidget_->refreshTable(); 
}

void 
PathFollowerInput::loadPreviousPath()
{
    pathUI_->loadPreviousUserPath( *guiPath_.get() );
    pathDesignTableWidget_->refreshTable(); 
}

void
PathFollowerInput::sendPath() 
{ 
    pathUI_->send(); 
}

void 
PathFollowerInput::cancelPath() 
{ 
    pathUI_->cancel(); 
}

void 
PathFollowerInput::switchTableView( bool onOff )
{
    if (onOff==true)
        pathDesignTableWidget_->show();
    else
        pathDesignTableWidget_->hide();
         
}

}
