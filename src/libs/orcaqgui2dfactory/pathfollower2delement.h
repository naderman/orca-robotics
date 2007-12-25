/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER2DELEMENT_H
#define PATHFOLLOWER2DELEMENT_H

#include <orca/pathfollower2d.h>

#include <hydroutil/store.h>
#include <hydroutil/timer.h>

#include <orcaqgui2d/guielement2d.h>
#include <orcaqgui2dfactory/pathpainter.h>
#include <orcaqgui2dfactory/pathinput.h>

namespace orcaqgui2d {

class hydroqgui::IHumanManager;

////////////////////////////////////////////////////////////////////////////////
// The consumer object. We need this here because PathFollower2dElement cannot inherit from IceStormElement.
// Reason is that PathFollower2dConsumer has several non-standard purely virtual member functions.
class PathUpdateConsumer : public orca::PathFollower2dConsumer
{
public:
    
    void setData( const orca::PathFollower2dData& newPath, const ::Ice::Current& );
    void setWaypointIndex( int index, const ::Ice::Current& );
    void setActivationTime( const orca::Time& absoluteTime, double relativeTime, const ::Ice::Current& );
    void setEnabledState( bool enabledState, const ::Ice::Current& );

    hydroutil::Store<orca::PathFollower2dData> pathPipe_;
    hydroutil::Store<int> indexPipe_;
};
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This class sets up all the buttons and actions for user interaction.
// It can be instantiated on the heap and deleted, Qt cleans up for us
class PathfollowerButtons : public QObject
{
    Q_OBJECT
            
public:
    PathfollowerButtons( QObject                       *parent,
                         hydroqgui::IHumanManager      *humanManager,
                         hydroqgui::ShortcutKeyManager *shortcutKeyManager,
                         std::string                    proxyString);
    ~PathfollowerButtons();
    
    void setWpButton( bool onOff );

private:
    QAction *hiWaypoints_;
    hydroqgui::IHumanManager *humanManager_;
    hydroqgui::ShortcutKeyManager *shortcutKeyManager_;
};
//////////////////////////////////////////////////////////////////////////////

class PathFollower2dElement;
//
// Handles the human-interface stuff for the PathFollower2dElement
//
class PathFollowerHI  : public QObject
{
    Q_OBJECT

public:
    PathFollowerHI( PathFollower2dElement *pfElement,
                    std::string proxyString,
                    hydroqgui::IHumanManager *humanManager,
                    hydroqgui::MouseEventManager *mouseEventManager,
                    hydroqgui::ShortcutKeyManager *shortcutKeyManager,
                    PathPainter &painter,
                    WaypointSettings wpSettings,
                    bool activateImmediately,
                    QString dumpPath );
    ~PathFollowerHI();

    void noLongerMouseEventReceiver();
    void paint( QPainter *p );

    void mousePressEvent(QMouseEvent *e) 
        {pathInput_->processPressEvent(e);}
    void mouseMoveEvent(QMouseEvent *e) {pathInput_->processMoveEvent(e);}
    void mouseReleaseEvent(QMouseEvent *e) {pathInput_->processReleaseEvent(e);}
    void mouseDoubleClickEvent(QMouseEvent *e) {pathInput_->processDoubleClickEvent(e);}
    void setFocus( bool inFocus );
    void setUseTransparency( bool useTransparency ); 
    
    // to dump the user (green) path to /tmp
    void savePath( const QString &fileName ) const
    {
        pathInput_->savePath( fileName );
    }

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
    hydroqgui::IHumanManager *humanManager_;
    hydroqgui::MouseEventManager *mouseEventManager_;
    hydroqgui::ShortcutKeyManager *shortcutKeyManager_;
    PathPainter   &painter_;

    QString pathFileName_;
    bool pathFileSet_;

    WaypointSettings wpSettings_;
    bool             activateImmediately_;

    PathFollowerInput *pathInput_;
    
    // sets up and destroys buttons and associated actions
    PathfollowerButtons *buttons_;

    // Do we own the global mode?
    bool gotMode_;
    
    bool useTransparency_;
    
    QString dumpPath_;
    int numPathDumps_;
    QString lastSavedPathFile_;
};

// We need to inherit from GuiElement2d, not from IceStormElement. 
// Reason is that PathFollower2dConsumer has a non-standard purely virtual 
// member function setWaypointIndex. 
// Disadvantage is that we have to subscribe ourselves.
class PathFollower2dElement : public GuiElement2d
{

public: 

    PathFollower2dElement( const orcaice::Context & context,
                           const std::string &proxyString,
                           hydroqgui::IHumanManager *humanManager,
                           hydroqgui::MouseEventManager *mouseEventManager,
                           hydroqgui::ShortcutKeyManager *shortcutKeyManager );
    ~PathFollower2dElement();

    // inherited from guielement
    void update();
    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const
        { return painter_.paintThisLayer(z); }
    
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor( color ); };
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); pathHI_.setFocus( inFocus); };
    virtual void setUseTransparency( bool useTransparency );

    virtual void noLongerMouseEventReceiver() { pathHI_.noLongerMouseEventReceiver(); }
    virtual void mousePressEvent(QMouseEvent *e) { pathHI_.mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { pathHI_.mouseMoveEvent(e); }
    virtual void mouseReleaseEvent(QMouseEvent *e) { pathHI_.mouseReleaseEvent(e); }
    virtual void mouseDoubleClickEvent(QMouseEvent *e) { pathHI_.mouseDoubleClickEvent(e); }

    void go();
    void stop();
    void sendPath( const PathFollowerInput &pathInput, bool activateImmediately );

private: 
    void doInitialSetup();

    PathPainter painter_;

    int connectToInterface();
    bool isConnected_;
    void getInitialData();

    PathUpdateConsumer *pathUpdateConsumer_;
    orca::PathFollower2dConsumerPrx callbackPrx_;
    orca::PathFollower2dPrx pathFollower2dPrx_;
    std::string proxyString_;

    orcaice::Context context_;
    
    hydroqgui::IHumanManager *humanManager_;
    hydroqgui::MouseEventManager *mouseEventManager_;
    hydroqgui::ShortcutKeyManager *shortcutKeyManager_;
    
    bool firstTime_;
    hydroutil::Timer *timer_;
    hydroutil::Timer *activationTimer_;
    
    // toggle states
    bool displayWaypoints_;
    bool displayPastWaypoints_;
    bool displayOlympicMarker_;
    bool currentTransparency_;
    
    bool isRemoteInterfaceSick_;
    
    // Handles human interface
    PathFollowerHI pathHI_;
    
    // returns 0 if remote call works otherwise -1
    int isFollowerEnabled( bool &isEnabled );
    
};

}

#endif
