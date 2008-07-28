/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER_HI_H
#define PATHPLANNER_HI_H

#include <memory>
#include <QObject>
#include <orcaqgui2dfactory/pathplannerbuttons.h>
#include <orcaqgui2dfactory/pathdesignscreen.h>
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
class PathPainter;

class PathPlannerHI  : public QObject
{
    Q_OBJECT

public:
    PathPlannerHI( PathPlanner2dElement                    *ppElement,
                   const std::string                       &proxyString,
                   hydroqguielementutil::IHumanManager     &humanManager,
                   hydroqguielementutil::MouseEventManager &mouseEventManager,
                   PathPainter                             &painter,
                   const orcaice::Context                  &context );

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

private:

    PathPlanner2dElement *ppElement_;
    std::string proxyString_;
    hydroqguielementutil::IHumanManager &humanManager_;
    hydroqguielementutil::MouseEventManager &mouseEventManager_;
    PathPainter &painter_;
    orcaice::Context context_;
    WaypointSettings wpSettings_;

    // saving the path which the pathplanner interface holds
    // (as opposed to the path the user enters in green)
    QString ifacePathFileName_;
    bool haveIfacePathFileName_;
    std::auto_ptr<PathFileHandler> ifacePathFileHandler_;

    // handles all user input through clicking, tables, etc.
    std::auto_ptr<PathPlannerInput> pathInput_;
    // sets up and destroys buttons and associated actions
    std::auto_ptr<PathplannerButtons> buttons_;
    
    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
};

}

#endif
