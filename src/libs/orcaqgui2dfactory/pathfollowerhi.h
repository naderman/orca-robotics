/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER_HI_H
#define PATHFOLLOWER_HI_H

#include <memory>
#include <QObject>
#include <orcaqgui2dfactory/pathfollowerbuttons.h>
#include <orcaqgui2dfactory/pathdesignscreen.h>
#include <orcaqgui2dfactory/pathfollowerinput.h>
#include <orcaqgui2dfactory/pathfilehandler.h>

namespace hydroqgui {
    class GuiElementSet;
}

namespace hydroqguielementutil {
    class MouseEventManager;
    class IHumanManager;
    class ShortcutKeyManager;
}

namespace orcaqgui2d {

class PathFollower2dElement;    
class PathPainter;
    
//
// Handles the human-interface stuff for the PathFollower2dElement
//
class PathFollowerHI  : public QObject
{
    Q_OBJECT

public:
    PathFollowerHI( PathFollower2dElement *pfElement,
                    std::string proxyString,
                    hydroqguielementutil::IHumanManager &humanManager,
                    hydroqguielementutil::MouseEventManager &mouseEventManager,
                    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                    const hydroqgui::GuiElementSet &guiElementSet,
                    const PathPainter &painter,
                    const WaypointSettings &wpSettings,
                    bool activateImmediately,
                    QString dumpPath );

    void noLongerMouseEventReceiver();
    void paint( QPainter *p );

    void mousePressEvent(QMouseEvent *e) 
        {pathInput_->processPressEvent(e);}
    void mouseMoveEvent(QMouseEvent *e) {pathInput_->processMoveEvent(e);}
    void mouseReleaseEvent(QMouseEvent *e) {pathInput_->processReleaseEvent(e);}
    void mouseDoubleClickEvent(QMouseEvent *e) {pathInput_->processDoubleClickEvent(e);}
    void setFocus( bool inFocus );
    void setUseTransparency( bool useTransparency ); 
    

public slots:
    void savePathAs();
    void savePath();
    void waypointSettingsDialog();
    void waypointModeSelected();
    void send();
    void cancel();
    void allGo();
    void allStop();
    void go();
    void stop();

private:

    PathFollower2dElement *pfElement_;
    std::string proxyString_;
    hydroqguielementutil::IHumanManager &humanManager_;
    hydroqguielementutil::MouseEventManager &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
    const hydroqgui::GuiElementSet &guiElementSet_;
    const PathPainter &painter_;

    QString pathFileName_;
    bool pathFileSet_;

    WaypointSettings wpSettings_;
    bool activateImmediately_;

    std::auto_ptr<PathFollowerInput> pathInput_;
    
    // sets up and destroys buttons and associated actions
    std::auto_ptr<PathfollowerButtons> buttons_;
    std::auto_ptr<PathFileHandler> pathFileHandler_;

    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
    
    QString dumpPath_;
    int numPathDumps_;
    QString lastSavedPathFile_;
};

} // end of namespace

#endif
