/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PATHFOLLOWER2DELEMENT_H
#define PATHFOLLOWER2DELEMENT_H

#include <orca/pathfollower2d.h>

#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <hydroqguielementutil/guielement2d.h>
#include <orcaqgui2dfactory/pathfollowerpainter.h>
#include <orcaqgui2dfactory/pathfolloweruserinteraction.h>

namespace orcaqgui2d {
    
class PathFollowerInput;
    
////////////////////////////////////////////////////////////////////////////////
// The consumer object. We need this here because PathFollower2dElement cannot inherit from IceStormGuiElement2d.
// Reason is that PathFollower2dConsumer has several non-standard purely virtual member functions.
class PathUpdateConsumer : public orca::PathFollower2dConsumer
{
public:
    
    void setData( const orca::PathFollower2dData& newPath, const ::Ice::Current& );
    void setState( const orca::PathFollower2dState& newState, const ::Ice::Current& );

    gbxiceutilacfr::Store<orca::PathFollower2dData> pathPipe_;
    gbxiceutilacfr::Store<orca::PathFollower2dState> statePipe_;
};
////////////////////////////////////////////////////////////////////////////////


// We need to inherit from GuiElement2d, not from IceStormGuiElement2d. 
// Reason is that PathFollower2dConsumer has a non-standard purely virtual 
// member function setState. 
// Disadvantage is that we have to subscribe ourselves.
class PathFollower2dElement : public hydroqguielementutil::GuiElement2d
{

public: 

    PathFollower2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                           const orcaice::Context                     &context,
                           const std::string                          &proxyString,
                           hydroqguielementutil::IHumanManager        &humanManager,
                           hydroqguielementutil::MouseEventManager    &mouseEventManager,
                           hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager,
                           const hydroqgui::GuiElementSet             &guiElementSet );

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
    virtual void setUseTransparency( bool useTransparency );

    virtual void noLongerMouseEventReceiver() { if ( pathUI_.get() ) pathUI_->noLongerMouseEventReceiver(); }
    virtual void mousePressEvent(QMouseEvent *e) { if ( pathUI_.get() ) pathUI_->mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { if ( pathUI_.get() ) pathUI_->mouseMoveEvent(e); }
    virtual void mouseReleaseEvent(QMouseEvent *e) { if ( pathUI_.get() ) pathUI_->mouseReleaseEvent(e); }
    virtual void mouseDoubleClickEvent(QMouseEvent *e) { if ( pathUI_.get() ) pathUI_->mouseDoubleClickEvent(e); }
    
    // sets an inputFactory different from the default one
    void setInputFactory ( std::auto_ptr<hydroqguipath::PathInputFactory> inputFactory );
    
    void go();
    void stop();
    void sendPath( const hydroqguipath::IPathInput *pathInput, 
                   bool                             activateImmediately );

    void enableHI();
    void disableHI();
    bool isHIEnabled() { return pathUI_.get() != 0; }

    // pathUI_ will call this
    void toggleEnabled();

    PathFollowerPainter &pathPainter() { return painter_; }

    // Toolbar buttons can be enabled and disabled.
    // By default they are enabled.
    void enableToolbarButtons( bool enable );
    
    // returns the PathUserInteraction object to hook up actions to it via signal-slot
    QObject* pathUi();

private: 

    PathFollowerPainter painter_;

    void connectToInterface();
    bool isConnected_;

    PathUpdateConsumer *pathUpdateConsumer_;
    orca::PathFollower2dConsumerPrx callbackPrx_;
    orca::PathFollower2dPrx pathFollower2dPrx_;
    std::string proxyString_;

    orcaice::Context context_;
    
    hydroqguielementutil::IHumanManager       &humanManager_;
    hydroqguielementutil::MouseEventManager   &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager  &shortcutKeyManager_;
    const hydroqgui::GuiElementSet            &guiElementSet_;
    
    std::auto_ptr<hydroqguipath::PathInputFactory> inputFactory_;
    
    bool firstTime_;
    gbxiceutilacfr::Timer timer_;
    
    bool isInFocus_;

    bool isRemoteInterfaceSick_;
    
    // Handles human interface
    std::auto_ptr<PathFollowerUserInteraction> pathUI_;
    
    // returns 0 if remote call works otherwise -1
    int isFollowerEnabled( bool &isEnabled );

    bool enableToolbarButtons_;
    
};

}

#endif
