/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER_UI_H
#define PATHFOLLOWER_UI_H

#include <memory>
#include <QObject>
#include <orcaice/context.h>
#include <hydroqguipath/pathdesignscreen.h>
#include <hydroqguipath/pathfollowerbuttons.h>
#include <hydroqguipath/ipathinput.h>
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
class PathFollowerPainter;
    
//!
//! Handles all user interaction with the PathFollower
//!
//! @author Tobias Kaupp
//!
class PathFollowerUserInteraction  : public QObject, 
                                     public hydroqguipath::IPathUserInteraction
{
    Q_OBJECT

public:
    PathFollowerUserInteraction( PathFollower2dElement                          *pfElement,
                                 const std::string                              &proxyString,
                                 hydroqguielementutil::IHumanManager            &humanManager,
                                 hydroqguielementutil::MouseEventManager        &mouseEventManager,
                                 hydroqguielementutil::ShortcutKeyManager       &shortcutKeyManager,
                                 const hydroqgui::GuiElementSet                 &guiElementSet,
                                 const PathFollowerPainter                      &painter,
                                 const orcaice::Context                         &context,
                                 const hydroqguipath::PathInputFactory          *inputFactory,
                                 bool                                           enableToolbarButtons = true );
    
    ~PathFollowerUserInteraction();

    void noLongerMouseEventReceiver();
    void paint( QPainter *p );
    void setFocus( bool inFocus );
    void setUseTransparency( bool useTransparency ); 

    void mousePressEvent(QMouseEvent *e);
    
    void mouseMoveEvent(QMouseEvent *e);
    
    void mouseReleaseEvent(QMouseEvent *e);
    
    void mouseDoubleClickEvent(QMouseEvent *e);

    void saveUserPath( const hydroqguipath::GuiPath &guiPath,
                         int numLoops,
                         float timeOffset );
    
    void loadUserPath(hydroqguipath::GuiPath &guiPath);
    void loadPreviousUserPath(hydroqguipath::GuiPath &guiPath);
    
    
public slots:
    void savePath();
    void savePathAs();
    void waypointSettingsDialog();
    void waypointModeToggled(bool);

    void send();
    void cancel();
    void allGo();
    void allStop();
    void go();
    void stop();
    void toggleEnabled();

private:

    PathFollower2dElement *pfElement_;
    std::string proxyString_;
    hydroqguielementutil::IHumanManager &humanManager_;
    hydroqguielementutil::MouseEventManager &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
    const hydroqgui::GuiElementSet &guiElementSet_;
    const PathFollowerPainter &painter_;
    orcaice::Context context_;
    const hydroqguipath::PathInputFactory *inputFactory_;
    hydroqguipath::WaypointSettings wpSettings_;

    // saving the path which the pathfollower *interface* holds
    QString ifacePathFileName_;
    bool haveIfacePathFileName_;
    std::auto_ptr<PathFileHandler> ifacePathFileHandler_;
    
    // saving the path which the user enters (green waypoints)
    QString userPathFileName_;
    bool haveUserPathFileName_;
    std::auto_ptr<PathFileHandler> userPathFileHandler_;
    
    // handles all user path input through clicking, tables, etc.
    std::auto_ptr<hydroqguipath::IPathInput> pathInput_;
    
    // automatic saving of paths on send
    int numAutoPathDumps_;
    
    // sets up and destroys buttons and associated actions
    std::auto_ptr<hydroqguipath::PathfollowerButtons> buttons_;

    // Do we own the mouse events?
    bool ownMouseEvents_;
    
    bool useTransparency_;

    bool enableToolbarButtons_;
    
};

} // end of namespace

#endif
