/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER_UI_H
#define PATHFOLLOWER_UI_H

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
// Handles all user interaction for the PathFollower2dElement
//
// Author: Tobias Kaupp
//
class PathFollowerUserInteraction  : public QObject
{
    Q_OBJECT

public:
    PathFollowerUserInteraction( PathFollower2dElement                    *pfElement,
                                 const std::string                        &proxyString,
                                 hydroqguielementutil::IHumanManager      &humanManager,
                                 hydroqguielementutil::MouseEventManager  &mouseEventManager,
                                 hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                 const hydroqgui::GuiElementSet           &guiElementSet,
                                 const PathPainter                        &painter,
                                 const orcaice::Context                   &context,
                                 const PathFollowerInputFactory           *inputFactory );

    void noLongerMouseEventReceiver();
    void paint( QPainter *p );
    void setFocus( bool inFocus );
    void setUseTransparency( bool useTransparency ); 

    void mousePressEvent(QMouseEvent *e) 
        {pathInput_->processPressEvent(e);}
    
    void mouseMoveEvent(QMouseEvent *e) 
        {pathInput_->processMoveEvent(e);}
    
    void mouseReleaseEvent(QMouseEvent *e) 
        {pathInput_->processReleaseEvent(e);}
    
    void mouseDoubleClickEvent(QMouseEvent *e) 
        {pathInput_->processDoubleClickEvent(e);}
    

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
    orcaice::Context context_;
    const PathFollowerInputFactory *inputFactory_;
    WaypointSettings wpSettings_;

    // saving the path which the pathfollower *interface* holds
    // (as opposed to the path the user enters in green)
    QString ifacePathFileName_;
    bool haveIfacePathFileName_;
    std::auto_ptr<PathFileHandler> ifacePathFileHandler_;
    
    // handles all user input through clicking, tables, etc.
    std::auto_ptr<IPathInput> pathInput_;
    
    // automatic saving of paths on send
    int numAutoPathDumps_;
    
    // remember the filename of the green user path
    QString loadPreviousPathFilename_;
    
    // sets up and destroys buttons and associated actions
    std::auto_ptr<PathfollowerButtons> buttons_;

    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
    
};

} // end of namespace

#endif
