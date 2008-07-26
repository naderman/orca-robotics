/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER_INPUT_H
#define PATHFOLLOWER_INPUT_H

#include <orcaqgui2dfactory/ipathinput.h>
#include <orcaqgui2dfactory/pathutils.h>
#include <orcaqgui2dfactory/pathdesignscreen.h>
#include <orcaqgui2dfactory/pathdesigntablewidget.h>
#include <orcaqgui2dfactory/pathfilehandler.h>


namespace orcaqgui2d {
    
class PathFollowerHI;

class PathFollowerInput : public IPathInput
{ 
            
    public:
        PathFollowerInput( PathFollowerHI                      &pathFollowerHI,
                           WaypointSettings                    *wpSettings,
                           hydroqguielementutil::IHumanManager &humanManager,
                           const QString                       &lastSavedPathFile );
        
        virtual ~PathFollowerInput() {};  
  
        virtual void paint( QPainter *painter );
        virtual void setUseTransparency( bool useTransparency );
        
        virtual void processPressEvent( QMouseEvent* e);
        virtual void processReleaseEvent( QMouseEvent* e );
        virtual void processDoubleClickEvent( QMouseEvent* e) {};
        virtual void processMoveEvent( QMouseEvent* e) {};
        
        virtual void updateWpSettings( WaypointSettings* wpSettings );
        
        virtual void savePath( const QString &filename );
        virtual void loadPath( const QString &filename );
        virtual void loadPreviousPath();
        
        virtual void sendPath();
        virtual void cancelPath();
        
        virtual void setWaypointFocus( int waypointId );

        bool getPath( orca::PathFollower2dData &pathData ) const;    
        
    private:
       PathFollowerHI &pathFollowerHI_;
       
       std::auto_ptr<PathDesignScreen> pathDesignScreen_;
       std::auto_ptr<PathDesignWidget> pathDesignWidget_;
       std::auto_ptr<GuiPath> guiPath_;
       std::auto_ptr<PathFileHandler> pathFileHandler_;
       
};

}

#endif
