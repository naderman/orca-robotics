/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER_HI_H
#define PATHPLANNER_HI_H

#include <memory>
#include <QObject>
#include <orcaice/context.h>
#include <hydroqguipath/pathdesignscreen.h>
#include <hydroqguipath/pathplannerbuttons.h>
#include <hydroqguipath/ipathinput.h>
#include <orcaqgui2dfactory/pathplannerinput.h>
    
namespace hydroqgui {
    class GuiElementSet;
}

namespace hydroqguielementutil {
    class MouseEventManager;
    class IHumanManager;
    class ShortcutKeyManager;
}

namespace orcaqgui2d {

class PathPlanner2dElement;
class PathPlannerPainter;

//!
//! Handles all user interaction with the PathPlanner
//!
//! @author Tobias Kaupp
//!
class PathPlannerUserInteraction  : public QObject, 
                                    public hydroqguipath::IPathUserInteraction
{
    Q_OBJECT

public:
    PathPlannerUserInteraction( PathPlanner2dElement                     *ppElement,
                                const std::string                        &proxyString,
                                hydroqguielementutil::IHumanManager      &humanManager,
                                hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                hydroqguielementutil::MouseEventManager  &mouseEventManager,
                                PathPlannerPainter                       &painter,
                                const orcaice::Context                   &context );
    
    ~PathPlannerUserInteraction();

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
    
    void saveUserPath( const hydroqguipath::GuiPath &guiPath,
                       int numLoops,
                       float timeOffset );
    
    void loadUserPath(hydroqguipath::GuiPath &guiPath);
    void loadPreviousUserPath(hydroqguipath::GuiPath &guiPath);

public slots:
    void savePathAs();
    void savePath();
    void waypointSettingsDialog();
    void waypointModeSelected();
    void send();
    void cancel();

private:

    PathPlanner2dElement *ppElement_;
    std::string proxyString_;
    hydroqguielementutil::IHumanManager &humanManager_;
    hydroqguielementutil::MouseEventManager &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
    PathPlannerPainter &painter_;
    orcaice::Context context_;
    hydroqguipath::WaypointSettings wpSettings_;

    // saving the path which the pathplanner *interface* holds
    QString ifacePathFileName_;
    bool haveIfacePathFileName_;
    std::auto_ptr<PathFileHandler> ifacePathFileHandler_;
    
    // saving the path which the user enters (green waypoints)
    QString userPathFileName_;
    bool haveUserPathFileName_;
    std::auto_ptr<PathFileHandler> userPathFileHandler_;

    // handles all user input through clicking, tables, etc.
    std::auto_ptr<PathPlannerInput> pathInput_;
    // sets up and destroys buttons and associated actions
    std::auto_ptr<hydroqguipath::PathplannerButtons> buttons_;
    
    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
};

}

#endif
