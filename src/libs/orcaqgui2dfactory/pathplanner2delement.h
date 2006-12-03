#if 0
/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef PATHPLANNER2DELEMENT_H
#define PATHPLANNER2DELEMENT_H

#include <orca/pathplanner2d.h>
#include <orcaice/ptrproxy.h>
#include <orcaice/timer.h>
#include <orcaice/context.h>
#include <orcaqgui2dfactory/pathpainter.h>
#include <orcaqgui2d/guielement2d.h>
#include <iostream>
#include <orcaqgui2dfactory/pathinput.h>

namespace orcaqgui {

class IHumanManager;

// There's two consumer objects: the first one for icestorm (pathplanner pushes out whatever it computed),
// the second one is for answers to tasks which were set by the GUI

////////////////////////////////////////////////////////////////////////////////
class PathPlannerUpdateConsumer : public orca::PathPlanner2dConsumer
{
public:
    
    void setData( const orca::PathPlanner2dDataPtr &newPath, const ::Ice::Current& );
    orcaice::PtrProxy<orca::PathPlanner2dDataPtr> pathPipe_;
};


class PathPlannerTaskAnswerConsumer : public orca::PathPlanner2dConsumer
{
    public:

        virtual void setData(const ::orca::PathPlanner2dDataPtr& data, const ::Ice::Current& = ::Ice::Current());
        orcaice::Proxy<QString> msgBuffer_;
};
////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////

// This class sets up all the buttons and actions for user interaction.
// It can be instantiated on the heap and deleted, Qt cleans up for us
class PathplannerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathplannerButtons( QObject *parent, IHumanManager *humanManager, std::string proxyString);
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
                   IHumanManager *humanManager,
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
    IHumanManager *humanManager_;
    PathPainter   &painter_;

    QString pathFileName_;
    bool pathFileSet_;

    WaypointSettings wpSettings_;

    PathPlannerInput *pathInput_;
    
    // sets up and destroys buttons and associated actions
    PathplannerButtons *buttons_;
    
    // Do we own the global mode?
    bool gotMode_;
};

//
// @author Tobias Kaupp
//
class PathPlanner2dElement : public GuiElement2d
{

public: 

    PathPlanner2dElement( const orcaice::Context context,
                          const std::string &proxyString,
                          IHumanManager* humanManager );
    ~PathPlanner2dElement();

    // inherited from guielement
    void update();
    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const
        { return painter_.paintThisLayer(z); }
    
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor( color ); };
    virtual void setFocus( bool inFocus );

    virtual void lostMode() { pathHI_.lostMode(); }
    virtual void mousePressEvent(QMouseEvent *e) { pathHI_.mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { pathHI_.mouseMoveEvent(e); }
    virtual void mouseReleaseEvent(QMouseEvent *e) { pathHI_.mouseReleaseEvent(e); }
    virtual void mouseDoubleClickEvent(QMouseEvent *e) { pathHI_.mouseDoubleClickEvent(e); }
    void sendPath( const PathPlannerInput &pathInput );

private: 

    void doInitialSetup();

    PathPainter painter_;
    
    bool doneInitialSetup_;
    
    // Task answer
    PathPlannerTaskAnswerConsumer *pathTaskAnswerConsumer_;
    orca::PathPlanner2dConsumerPrx taskCallbackPrx_;
    Ice::ObjectPtr pathPlanner2dConsumerObj_;
    
    // Icestorm
    PathPlannerUpdateConsumer *pathUpdateConsumer_;
    orca::PathPlanner2dConsumerPrx callbackPrx_;
    
    orca::PathPlanner2dPrx pathPlanner2dPrx_;
    
    orcaice::Context context_;
    std::string proxyString_;
    IHumanManager *humanManager_;
    
    bool firstTime_;
    orcaice::Timer *timer_;

    // Handles human interface
    PathPlannerHI pathHI_;
};

}

#endif
#endif
