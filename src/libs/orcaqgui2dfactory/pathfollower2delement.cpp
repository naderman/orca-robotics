/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/pathfollower2d.h>
#include <hydroqguielementutil/ihumanmanager.h>

#include "pathconversionutil.h"
#include "pathfollower2delement.h"

using namespace std;

namespace orcaqgui2d {

void
PathUpdateConsumer::setData( const orca::PathFollower2dData& newPath, const ::Ice::Current& )
{
    pathPipe_.set( newPath );
}

void
PathUpdateConsumer::setState( const orca::PathFollower2dState& newState, const ::Ice::Current& )
{
    statePipe_.set( newState );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PathFollower2dElement::PathFollower2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                              const orcaice::Context                     &context,
                                              const std::string                          &proxyString,
                                              hydroqguielementutil::IHumanManager        &humanManager,
                                              hydroqguielementutil::MouseEventManager    &mouseEventManager,
                                              hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager,
                                              const hydroqgui::GuiElementSet             &guiElementSet )
    : GuiElement2d(guiElementInfo),
      isConnected_(false),
      proxyString_(proxyString),
      context_(context),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      shortcutKeyManager_(shortcutKeyManager),
      guiElementSet_(guiElementSet),
      firstTime_(true),
      isInFocus_(false),
      isRemoteInterfaceSick_(false),
      enableToolbarButtons_(true)
{ 
    inputFactory_.reset( new PathFollowerInputFactory );
    enableHI();

    PathFollowerPainterConfig config;
    painter_.initialize( config );
    
    pathUpdateConsumer_ = new PathUpdateConsumer;

    connectToInterface();
}

void
PathFollower2dElement::setInputFactory ( std::auto_ptr<hydroqguipath::PathInputFactory> inputFactory )
{
    disableHI();
    inputFactory_ = inputFactory;
    enableHI();
}

void
PathFollower2dElement::enableHI()
{
    pathUI_.reset( new PathFollowerUserInteraction( this,
                                                    proxyString_,
                                                    humanManager_,
                                                    mouseEventManager_,
                                                    shortcutKeyManager_,
                                                    guiElementSet_,
                                                    painter_, 
                                                    context_,
                                                    inputFactory_.get(),
                                                    enableToolbarButtons_ ) );
    pathUI_->setFocus( isInFocus_ );
    pathUI_->setUseTransparency( painter_.useTransparency() );
}

void
PathFollower2dElement::disableHI()
{
    pathUI_.reset( 0 );
}

void
PathFollower2dElement::setFocus( bool inFocus ) 
{
    painter_.setFocus( inFocus );
    isInFocus_ = inFocus;
    
    if ( pathUI_.get() )
        pathUI_->setFocus( inFocus); 
};

void
PathFollower2dElement::update()
{
    // if initial setup is not done yet (no connection established)
    // then try to connect every 5 seconds
    if ( !isConnected_ )
    {
        if (firstTime_) {
            connectToInterface();
            timer_.restart();
            firstTime_ = false;
        }
        if (timer_.elapsedSec() > 5.0) {
            connectToInterface();
            timer_.restart();
        }
    }
    if ( !isConnected_ ) return;
    
    if ( pathUpdateConsumer_->pathPipe_.isNewData() )
    {
        orca::PathFollower2dData newPath;
        pathUpdateConsumer_->pathPipe_.get( newPath );
        painter_.setData( newPath );
    }
    if ( pathUpdateConsumer_->statePipe_.isNewData() )
    {
        orca::PathFollower2dState state;
        pathUpdateConsumer_->statePipe_.get( state );
        painter_.setState( state );
    }
}

int 
PathFollower2dElement::isFollowerEnabled( bool &isEnabled )
{
    try
    {
        isEnabled = pathFollower2dPrx_->getState().isEnabled;
        if (isRemoteInterfaceSick_) {
            humanManager_.showStatusInformation( "PathFollower2dElement::remote interface is sane again" );
            isRemoteInterfaceSick_ = false;
        }
        return 0;
    }
    catch ( ... )
    {
        if (!isRemoteInterfaceSick_) {
            humanManager_.showStatusError(  "PathFollower2dElement::remote interface seems sick" );
            isRemoteInterfaceSick_ = true;
        }
        return -1;
    }
}

void 
PathFollower2dElement::setUseTransparency( bool useTransparency ) 
{ 
//     cout << "TRACE(pathfollower2delement.cpp): setUseTransparency: " << useTransparency << endl;
    painter_.setUseTransparency( useTransparency );

    if ( pathUI_.get() )
        pathUI_->setUseTransparency( useTransparency );
}

void
PathFollower2dElement::connectToInterface()
{
    if (isConnected_) return;
    
    humanManager_.showStatusInformation( "PathFollowerElement is trying to connect");
    
    // Here's what IceStormGuiElement2d usually does for you if the GuiElement inherits from IceStormElement (see comments in .h file for more information)
    try 
    {
        orcaice::connectToInterfaceWithString( context_, pathFollower2dPrx_, proxyString_ );
        Ice::ObjectPtr pathFollowerObj = pathUpdateConsumer_;
        
        // subscribe
        callbackPrx_ = orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( context_, pathFollowerObj );
        pathFollower2dPrx_->subscribe(callbackPrx_);
        humanManager_.showStatusInformation( "Connected to pathfollower interface successfully.");
        isConnected_ = true;
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement:: Problem connecting to pathfollower interface: " << e.what();
        humanManager_.showStatusWarning( ss.str().c_str() );
    }
    catch ( ... )
    {
        humanManager_.showStatusWarning( "PathFollower2dElement: Unknown problem connecting to pathfollower interface.");
    }
}

QStringList
PathFollower2dElement::contextMenu()
{
    QStringList s;
    if (painter_.displayWaypoints()) {
        s << "Switch all waypoints OFF";
    } else {
        s << "Switch all waypoints ON";
    }
    if (painter_.displayPastWaypoints()) {
        s << "Switch past waypoints OFF";
    } else {
        s << "Switch past waypoints ON";
    }
    if (painter_.displayFutureWaypoints()) {
        s << "Switch future waypoints OFF";
    } else {
        s << "Switch future waypoints ON";
    }
    if (painter_.displayPathWhenFinished()) {
        s << "Switch finished path OFF";
    } else {
        s << "Switch finished path ON";
    }
    if (painter_.displayPathWhenNotActivated()) {
        s << "Switch unactivated path OFF";
    } else {
        s << "Switch unactivated path ON";
    }
    if (painter_.displayOlympicMarker()) {
        s << "Switch olympic marker OFF";
    } else {
        s << "Switch olympic marker ON";
    }
    if (painter_.useTransparency()) {
        s << "Switch transparency OFF";
    } else {
        s << "Switch transparency ON";
    }
    bool isEnabled;
    int ret = isFollowerEnabled(isEnabled);
    if (ret!=0) {
        s << "";
    } else {
        if (isEnabled) {
            s << "Disable interface";
        } else {
            s << "Enable interface";
        }
    }
    
    s << "Save path as..."
      << "Save path";
    return s;
}

void 
PathFollower2dElement::execute( int action )
{
//     cout<<"TRACE(pathfollower2delement.cpp): execute: " << action << endl;
    if ( action == 0 )
        painter_.toggleDisplayWaypoints();    
    else if ( action == 1 )
        painter_.togglePastWaypoints();
    else if ( action == 2 )
        painter_.toggleFutureWaypoints();
    else if ( action == 3 )
        painter_.toggleDisplayPathWhenFinished();
    else if ( action == 4 )
        painter_.toggleDisplayPathWhenNotActivate();
    else if ( action == 5 )
        painter_.toggleOlympicMarker();
    else if ( action == 6 )
        setUseTransparency(!painter_.useTransparency());
    else if ( action == 7 )
        toggleEnabled();
    else if ( action == 8 )
    {
        if ( pathUI_.get() ) pathUI_->savePathAs();
    }
    else if ( action == 9 )
    {
        if ( pathUI_.get() ) pathUI_->savePath();
    }
    else
        assert( false && "PathFollower2dElement::execute(): bad action" );
}

void
PathFollower2dElement::toggleEnabled()
{
    if ( !pathFollower2dPrx_ ) {
        humanManager_.showStatusInformation("Proxy to PathFollower2d interface is not initialized");
        return;
    }

    if (isRemoteInterfaceSick_) return;

    bool isEnabled;
    try {
        orca::PathFollower2dState state = pathFollower2dPrx_->getState();
        state.isEnabled = !state.isEnabled;
        pathFollower2dPrx_->setEnabled( state.isEnabled );
        isEnabled = state.isEnabled;
        isFollowerEnabled( isEnabled );
    }
    catch ( const std::exception &e ) {
        stringstream ss;
        ss << "While trying to toggle enabled status: " << e.what();
        humanManager_.showStatusError(  ss.str().c_str() );
    }

    QString str;
    if (isEnabled) {
        str = "Pathfollower reports it is ENABLED now.";
    } else {
        str = "Pathfollower reports it is DISABLED now.";
    }
    humanManager_.showStatusInformation(str);
}

void 
PathFollower2dElement::go()
{
    if (!pathFollower2dPrx_) {
        humanManager_.showStatusError("Proxy to pathfollower2d interface is not intitialized");
        return;
    }
    
//     cout<<"TRACE(PathFollower2dElement): go()" << endl;
    humanManager_.showStatusInformation("Received GO signal");
    try
    {
        pathFollower2dPrx_->activateNow();
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "While trying to activate pathfollower: " << endl << e.what();
        humanManager_.showStatusError( QString(ss.str().c_str()) );
    }
}

void 
PathFollower2dElement::stop()
{
    if (!pathFollower2dPrx_) {
        humanManager_.showStatusError("Proxy to pathfollower2d interface is not intitialized");
        return;
    }
        
//     cout<<"TRACE(PathFollower2dElement): stop()" << endl;
    humanManager_.showStatusInformation("Received STOP signal");
    orca::PathFollower2dData dummyPath;
    const bool activateNow = true;
    try
    {
        pathFollower2dPrx_->setData( dummyPath, activateNow );
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "While trying to send empty path: " << endl << e.what();
        humanManager_.showStatusError( QString(ss.str().c_str()) );
    }
}

void 
PathFollower2dElement::sendPath( const hydroqguipath::IPathInput *pathInput, 
                                 bool                             activateImmediately )
{
    if (!pathFollower2dPrx_) {
        humanManager_.showStatusError("Proxy to pathfollower2d interface is not intitialized");
        return;
    }
    
    try
    {
        // it's possible that we were desubscribed before, let's resubscribe to make sure
        // this is useful to get instant feedback in the form of a displayed path coming back from localnav
        // if we are already subscribed, the server will just do nothing
        pathFollower2dPrx_->subscribe(callbackPrx_);
        
        hydroqguipath::GuiPath guiPath;
        int numLoops = 0;
        float timeOffset = 0.0;
        pathInput->getPath( guiPath, numLoops, timeOffset );
        
        if ( guiPath.size()>0 )
        {

            orca::PathFollower2dData data;
            data.timeStamp = orcaice::getNow();
            guiPathToOrcaPath( guiPath, data.path, numLoops, timeOffset );
//             cout<<"TRACE(pathfollower2delement.cpp): setData() with data: " << orcaobj::toVerboseString(data) << endl;
            pathFollower2dPrx_->setData( data, activateImmediately );
            if (!activateImmediately) 
                humanManager_.showStatusInformation( "Path needs to be activated by pressing the Go button." );
        } 
        else 
        {
            humanManager_.showStatusWarning( "No path to send!" );
        }        
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "Error while trying to send pathfollowing data: " << e.what();
        humanManager_.showStatusError( ss.str().c_str() );
    }
}

void 
PathFollower2dElement::paint( QPainter *p, int z )
{   
    painter_.paint( p, z );

    if ( pathUI_.get() )
        pathUI_->paint( p );
}

void
PathFollower2dElement::enableToolbarButtons( bool enable )
{
    disableHI();
    enableToolbarButtons_ = enable;
    enableHI();
}

QObject*
PathFollower2dElement::pathUi()
{
    if ( pathUI_.get() )
        return (QObject*)pathUI_.get();
    
    return 0;
}

}
