/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguielementutil/shortcutkeymanager.h>
#include <hydroqgui/guiicons.h>
#include "pathfollowerbuttons.h"

using namespace std;

namespace orcaqgui2d {

PathfollowerButtons::PathfollowerButtons( QObject                                   *parent,
                                          hydroqguielementutil::IHumanManager       &humanManager,
                                          hydroqguielementutil::ShortcutKeyManager  &shortcutKeyManager,
                                          string                                     proxyString)
    : humanManager_(humanManager),
      shortcutKeyManager_(shortcutKeyManager)
{
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);

    QPixmap waypointsIcon(waypoints_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

    QAction* fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path As", this );
    connect(fileSavePathAs, SIGNAL(triggered()), parent, SLOT(savePathAs()));
    QAction* fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path", this );
    connect(fileSavePath, SIGNAL(triggered()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(waypointsIcon, QString(proxyString.c_str()) + "\n" + "&Pathfollower waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(triggered()), parent, SLOT(waypointModeSelected()) );

    QAction* hiSend = new QAction(sendIcon,QString(proxyString.c_str()) + "\n" + "&Send PathFollower Path", this);
    connect( hiSend,SIGNAL(triggered()), parent, SLOT(send()) );
    QAction* hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "&Discard PathFollower Path", this);
    connect( hiCancel,SIGNAL(triggered()), parent ,SLOT(cancel()) );

    QAction* hiGo = new QAction(goIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Go All Robots", this);
    connect( hiGo, SIGNAL(triggered()), parent, SLOT(allGo()) ); 
    QAction* hiStop = new QAction(stopIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Stop All Robots", this);
    connect( hiStop, SIGNAL(triggered()), parent, SLOT(allStop()) ); 

    humanManager.fileMenu()->addAction(fileSavePathAs);
    humanManager.fileMenu()->addAction(fileSavePath);

    shortcutKeyManager.subscribeToShortcutKey( hiStop, QKeySequence(Qt::Key_Escape), false, this );
    humanManager.toolBar()->addAction(hiStop);
    shortcutKeyManager.subscribeToShortcutKey( hiWaypoints_, QKeySequence(Qt::Key_F10), true, this );
    humanManager.toolBar()->addAction(hiWaypoints_);
    shortcutKeyManager.subscribeToShortcutKey( hiSend, QKeySequence(Qt::Key_F11), true, this );
    humanManager.toolBar()->addAction(hiSend);
    shortcutKeyManager.subscribeToShortcutKey( hiGo, QKeySequence(Qt::Key_F12), false, this );
    humanManager.toolBar()->addAction(hiGo);
    humanManager.toolBar()->addAction( hiCancel );

    QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "&PathFollower Waypoint settings", this );
    connect( wpDialogAction, SIGNAL(triggered()), parent, SLOT(waypointSettingsDialog()) );
    humanManager.optionsMenu()->addAction( wpDialogAction );

    QAction *sep = humanManager.toolBar()->addSeparator();
    sep->setParent( this );
}

PathfollowerButtons::~PathfollowerButtons() 
{
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_Escape), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F10), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F11), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F12), this ); 
}

void 
PathfollowerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}

}

