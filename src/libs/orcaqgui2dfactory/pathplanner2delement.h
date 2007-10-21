/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER2DELEMENT_H
#define PATHPLANNER2DELEMENT_H

#include <iostream>
#include <orca/pathplanner2d.h>
#include <hydroutil/proxy.h>
#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui2d/guielement2d.h>
#include <orcaqgui2dfactory/pathpainter.h>
#include <orcaqgui2dfactory/pathinput.h>

namespace orcaqgui2d {

class orcaqgui::IHumanManager;

// There's two consumer objects: the first one for icestorm (pathplanner pushes out whatever it computed) which is part
// of the base class IceStormElement.
// The second one here is for answers to tasks which were set by the GUI.

////////////////////////////////////////////////////////////////////////////////
class PathPlannerTaskAnswerConsumer : public orca::PathPlanner2dConsumer
{
    public:

        virtual void setData(const ::orca::PathPlanner2dData& data, const ::Ice::Current& = ::Ice::Current());
        hydroutil::Proxy<QString> msgProxy_;
};
////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////

// This class sets up all the buttons and actions for user interaction.
// It can be instantiated on the heap and deleted, Qt cleans up for us
class PathplannerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathplannerButtons( QObject *parent, orcaqgui::IHumanManager *humanManager, std::string proxyString);
    ~PathplannerButtons() 
    { 
        // Qt cleans up for us 
    };
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
};
/////////////////////////////////////////

class PathPlanner2dElement;
class PathPlannerHI  : public QObject
{
    Q_OBJECT

public:
    PathPlannerHI( PathPlanner2dElement *ppElement,
                   std::string proxyString,
                   orcaqgui::IHumanManager *humanManager,
                   PathPainter &painter,
                   WaypointSettings wpSettings );
    ~PathPlannerHI();

    void lostMode();
    void paint( QPainter *p );

    void mousePressEvent(QMouseEvent *e) 
        {pathInput_->processPressEvent(e);}
    void mouseMoveEvent(QMouseEvent *e) {pathInput_->processMoveEvent(e);}
    void mouseReleaseEvent(QMouseEvent *e) {pathInput_->processReleaseEvent(e);}
    void mouseDoubleClickEvent(QMouseEvent *e) {pathInput_->processDoubleClickEvent(e);}
    void setFocus( bool inFocus );
    void setTransparency( bool useTransparency ); 

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
    orcaqgui::IHumanManager *humanManager_;
    PathPainter   &painter_;

    QString pathFileName_;
    bool pathFileSet_;

    WaypointSettings wpSettings_;

    PathPlannerInput *pathInput_;
    
    // sets up and destroys buttons and associated actions
    PathplannerButtons *buttons_;
    
    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
};

//
// @author Tobias Kaupp
//
class PathPlanner2dElement : public orcaqgui2d::IceStormElement<PathPainter,
                             orca::PathPlanner2dData,
                             orca::PathPlanner2dPrx,
                             orca::PathPlanner2dConsumer,
                             orca::PathPlanner2dConsumerPrx>
{

public: 

    PathPlanner2dElement( const orcaice::Context & context,
                          const std::string &proxyString,
                          orcaqgui::IHumanManager* humanManager );
    ~PathPlanner2dElement();

    void update();
    
    // overriding paint since we have to paint human input
    void paint( QPainter *p, int z );
    
    virtual void actionOnConnection();
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor( color ); };
    virtual void setFocus( bool inFocus );
    virtual void setTransparency( bool useTransparency );

    virtual void lostMode() { pathHI_.lostMode(); }
    virtual void mousePressEvent(QMouseEvent *e) { pathHI_.mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { pathHI_.mouseMoveEvent(e); }
    virtual void mouseReleaseEvent(QMouseEvent *e) { pathHI_.mouseReleaseEvent(e); }
    virtual void mouseDoubleClickEvent(QMouseEvent *e) { pathHI_.mouseDoubleClickEvent(e); }
    void sendPath( const PathPlannerInput &pathInput );

private: 

    PathPainter painter_;
    
    // Task answer
    PathPlannerTaskAnswerConsumer *pathTaskAnswerConsumer_;
    orca::PathPlanner2dConsumerPrx taskCallbackPrx_;
    Ice::ObjectPtr pathPlanner2dConsumerObj_;
    
    orca::PathPlanner2dPrx pathPlanner2dPrx_;
    
    orcaice::Context context_;
    std::string proxyString_;
    orcaqgui::IHumanManager *humanManager_;

    bool displayWaypoints_;
    bool currentTransparency_;
        
    // Handles human interface
    PathPlannerHI pathHI_;

};

}

#endif
