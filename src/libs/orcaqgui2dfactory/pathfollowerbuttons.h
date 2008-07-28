/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER2D_BUTTONS_H
#define PATHFOLLOWER2D_BUTTONS_H

#include <QObject>

class QAction;

namespace hydroqguielementutil {
    class ShortcutKeyManager;
    class IHumanManager;   
}

namespace orcaqgui2d {

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
                             std::string                               proxyString);
        ~PathfollowerButtons();
    
        void setWpButton( bool onOff );

    private:
        QAction *hiWaypoints_;
        hydroqguielementutil::IHumanManager &humanManager_;
        hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
};

}

#endif
