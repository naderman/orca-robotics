/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER2D_BUTTONS_H
#define PATHFOLLOWER2D_BUTTONS_H

#include <QObject>
#include <memory>
#include <hydroqguielementutil/shortcutkeymanager.h>

class QAction;

namespace hydroqguielementutil {
    class IHumanManager;   
}

namespace hydroqguipath {

//
// This class sets up all the buttons and actions user interaction with the pathfollower.
//
// Author: Tobias Kaupp
//
class PathfollowerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathfollowerButtons( QObject                                  *parent,
                         hydroqguielementutil::IHumanManager      &humanManager,
                         hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                         std::string                               proxyString,
                         bool                                      haveActivatePathButton=true);
    
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
    hydroqguielementutil::IHumanManager &humanManager_;

    std::auto_ptr<hydroqguielementutil::ShortcutKeyReservation> stopShortcutKey_;
    std::auto_ptr<hydroqguielementutil::ShortcutKeyReservation> drawWaypointsShortcutKey_;
    std::auto_ptr<hydroqguielementutil::ShortcutKeyReservation> sendPathShortcutKey_;
    std::auto_ptr<hydroqguielementutil::ShortcutKeyReservation> activatePathShortcutKey_;
};

}

#endif
