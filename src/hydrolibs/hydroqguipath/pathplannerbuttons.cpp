#include <iostream>
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguielementutil/shortcutkeymanager.h>
#include <hydroqgui/guiicons.h>
#include "pathplannerbuttons.h"

namespace hydroqguipath {

PathplannerButtons::PathplannerButtons( QObject                                  *parent, 
                                        hydroqguielementutil::IHumanManager      &humanManager, 
                                        hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                        const std::string                        &proxyString)
    : shortcutKeyManager_(shortcutKeyManager)
{
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);

    QPixmap globalwpIcon(globalwaypoint_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

    QAction *fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathPlanner Path As", this );
    connect(fileSavePathAs, SIGNAL(triggered()), parent, SLOT(savePathAs()));
    QAction *fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathPlanner Path", this );
    connect(fileSavePath, SIGNAL(triggered()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(globalwpIcon, QString(proxyString.c_str()) + "\n" + "PathPlanner waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(triggered()), parent, SLOT(waypointModeSelected()) );

    QAction *hiSend = new QAction(sendIcon, QString(proxyString.c_str()) + "\n" + "Send PathPlanner Task", this);
    connect( hiSend,SIGNAL(triggered()), parent, SLOT(send()) );
    QAction *hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "Discard PathPlanner Task", this);
    connect( hiCancel,SIGNAL(triggered()), parent ,SLOT(cancel()) );
    
    QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "PathPlanner Waypoint settings", this );
    connect( wpDialogAction, SIGNAL(triggered()), parent, SLOT(waypointSettingsDialog()) );

    humanManager.fileMenu()->addAction(fileSavePathAs);
    humanManager.fileMenu()->addAction(fileSavePath);

//     humanManager.toolBar()->addAction(fileSavePathAs);
//     humanManager.toolBar()->addAction(fileSavePath);

    shortcutKeyManager.subscribeToShortcutKey( hiWaypoints_, QKeySequence(Qt::Key_F10), true, this );
    humanManager.toolBar()->addAction( hiWaypoints_ );
    shortcutKeyManager.subscribeToShortcutKey( hiSend, QKeySequence(Qt::Key_F11), true, this );
    humanManager.toolBar()->addAction( hiSend );
    humanManager.toolBar()->addAction( hiCancel );
    
    humanManager.optionsMenu()->addAction( wpDialogAction );
    
    QAction *sep = humanManager.toolBar()->addSeparator();
    sep->setParent( this );
}

PathplannerButtons::~PathplannerButtons() 
{
    std::cout << "PathplannerButtons: destructor: Unsubscribing shortcutkeymanagers. " << std::endl;
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F10), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F11), this ); 
}

void 
PathplannerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}

}
