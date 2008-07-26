/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER_BUTTONS_H
#define PATHPLANNER_BUTTONS_H

#include <QObject>

class QAction;

namespace hydroqguielementutil {
    class ShortcutKeyManager;
    class IHumanManager;   
}

namespace orcaqgui2d {

// This class sets up all the buttons and actions for user interaction.
// It can be instantiated on the heap and deleted, Qt cleans up for us
class PathplannerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathplannerButtons( QObject                             *parent, 
                        hydroqguielementutil::IHumanManager &humanManager, 
                        const std::string                   &proxyString);
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
};

}

#endif
