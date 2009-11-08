/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqgui/guiicons.h>
#include "pathfollowerbuttons.h"

using namespace std;

namespace hydroqguipath {

PathfollowerButtons::PathfollowerButtons( QObject                                   *parent,
                                          hydroqguielementutil::IHumanManager       &humanManager,
                                          hydroqguielementutil::ShortcutKeyManager  &shortcutKeyManager,
                                          string                                     proxyString,
                                          bool                                       haveActivatePathButton )
    : humanManager_(humanManager)
{
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);

    QPixmap waypointsIcon(waypoints_xpm);
    QPixmap zoomIcon(fileopen_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

    QAction* fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path As", this );
    connect(fileSavePathAs, SIGNAL(triggered()), parent, SLOT(savePathAs()));
    QAction* fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path", this );
    connect(fileSavePath, SIGNAL(triggered()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(waypointsIcon, QString(proxyString.c_str()) + "\n" + "&Pathfollower waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(toggled(bool)), parent, SLOT(waypointModeToggled(bool)) );

    QAction* hiSend = new QAction(sendIcon,QString(proxyString.c_str()) + "\n" + "&Send PathFollower Path", this);
    connect( hiSend,SIGNAL(triggered()), parent, SLOT(send()) );
    QAction* hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "&Discard PathFollower Path", this);
    connect( hiCancel,SIGNAL(triggered()), parent ,SLOT(cancel()) );

    QAction* hiActivatePath = 0;
    if (haveActivatePathButton)
    {
        hiActivatePath = new QAction(goIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower ActivatePath All Robots", this);
        connect( hiActivatePath, SIGNAL(triggered()), parent, SLOT(allGo()) ); 
    }
    
    QAction* hiStop = new QAction(stopIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Stop All Robots", this);
    connect( hiStop, SIGNAL(triggered()), parent, SLOT(allStop()) );

    if ( humanManager_.fileMenu() ) {
        humanManager_.fileMenu()->addAction(fileSavePathAs);
        humanManager_.fileMenu()->addAction(fileSavePath);
    }
    
    if ( humanManager_.toolBar() )
    {
        stopShortcutKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( hiStop,
                                                                                  QKeySequence(Qt::Key_Escape),
                                                                                  false,
                                                                                  this,
                                                                                  shortcutKeyManager ) );
        humanManager_.toolBar()->addAction(hiStop);

        drawWaypointsShortcutKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( hiWaypoints_,
                                                                                           QKeySequence(Qt::Key_F10),
                                                                                           true,
                                                                                           this,
                                                                                           shortcutKeyManager ) );
        humanManager_.toolBar()->addAction(hiWaypoints_);

        sendPathShortcutKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( hiSend,
                                                                                      QKeySequence(Qt::Key_F11),
                                                                                      true,
                                                                                      this,
                                                                                      shortcutKeyManager ) );
        humanManager_.toolBar()->addAction(hiSend);

        if (haveActivatePathButton)
        {
            activatePathShortcutKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( hiActivatePath,
                                                                                              QKeySequence(Qt::Key_F12),
                                                                                              false,
                                                                                              this,
                                                                                              shortcutKeyManager ) );
            humanManager_.toolBar()->addAction(hiActivatePath);
        }
        humanManager_.toolBar()->addAction( hiCancel );
        
        QAction *sep = humanManager_.toolBar()->addSeparator();
        sep->setParent( this );
    }
    
    if ( humanManager_.optionsMenu() ) {
        QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "&PathFollower Waypoint settings", this );
        connect( wpDialogAction, SIGNAL(triggered()), parent, SLOT(waypointSettingsDialog()) );
        humanManager_.optionsMenu()->addAction( wpDialogAction );
    }
}

void 
PathfollowerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}

}

