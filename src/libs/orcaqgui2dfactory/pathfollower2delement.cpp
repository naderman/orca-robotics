/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/stringutils.h>
#include <hydroqguielementutil/ihumanmanager.h>

#include "pathfollower2delement.h"

using namespace std;

namespace orcaqgui2d {

void
PathUpdateConsumer::setData( const orca::PathFollower2dData& newPath, const ::Ice::Current& )
{
    pathPipe_.set( newPath );
}

void
PathUpdateConsumer::setWaypointIndex( int index, const ::Ice::Current& )
{
    indexPipe_.set( index );
}

void PathUpdateConsumer::setActivationTime( const orca::Time& absoluteTime, double relativeTime, const ::Ice::Current& )
{
    static bool _havePrintedDbug(false);
    if (!_havePrintedDbug)
    {
        cout << "PathFollower2d: got a new activation time. Not used, we rely on getData calls." << endl;
        cout << "That warning is only printed once." << endl;
        _havePrintedDbug = true;
    }
}

void PathUpdateConsumer::setEnabledState( bool enabledState, const ::Ice::Current& )
{
    cout << "PathFollower2d: enable state changed. Not used, we rely on getData calls." << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PathFollower2dElement::PathFollower2dElement( const orcaice::Context &context,
                                              const std::string &proxyString,
                                              hydroqguielementutil::IHumanManager &humanManager,
                                              hydroqguielementutil::MouseEventManager &mouseEventManager,
                                              hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                              const hydroqgui::GuiElementSet &guiElementSet )
    : isConnected_(false),
      proxyString_(proxyString),
      context_(context),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      shortcutKeyManager_(shortcutKeyManager),
      guiElementSet_(guiElementSet),
      firstTime_(true),
      displayWaypoints_(true),
      displayPastWaypoints_(false),
      displayFutureWaypoints_(true),
      displayOlympicMarker_(true),
      currentTransparency_(false),
      isInFocus_(false),
      isRemoteInterfaceSick_(false)
{
    cout<<"TRACE(pathfollower2delement.cpp): Instantiating w/ proxyString '" << proxyString << "'" << endl;
    enableHI();
    
    painter_.initialize( displayWaypoints_, displayPastWaypoints_, displayFutureWaypoints_, displayOlympicMarker_, currentTransparency_);
    
    pathUpdateConsumer_ = new PathUpdateConsumer;

    doInitialSetup();
    
    timer_ = new gbxiceutilacfr::Timer;
    activationTimer_ = new gbxiceutilacfr::Timer;
    activationTimer_->restart();
}

void
PathFollower2dElement::enableHI()
{
    pathHI_.reset( new PathFollowerUserInteraction( this,
                                                    proxyString_,
                                                    humanManager_,
                                                    mouseEventManager_,
                                                    shortcutKeyManager_,
                                                    guiElementSet_,
                                                    painter_, 
                                                    context_ ) );
    pathHI_->setFocus( isInFocus_ );
    pathHI_->setUseTransparency( currentTransparency_ );
}

void
PathFollower2dElement::disableHI()
{
    pathHI_.reset( 0 );
}

void
PathFollower2dElement::setFocus( bool inFocus ) 
{
    painter_.setFocus( inFocus ); 
    if ( pathHI_.get() )
        pathHI_->setFocus( inFocus); 
    isInFocus_ = inFocus;
};

void
PathFollower2dElement::update()
{
    // if initial setup is not done yet (no connection established)
    // then try to connect every 5 seconds
    if ( !isConnected_ )
    {
        if (firstTime_) {
            doInitialSetup();
            timer_->restart();
            firstTime_=false;
        }
        if (timer_->elapsedSec()>5.0) {
            doInitialSetup();
            timer_->restart();
        }
    }
    if ( !isConnected_ ) return;
    
    if ( pathUpdateConsumer_->pathPipe_.isNewData() )
    {
        orca::PathFollower2dData newPath;
        pathUpdateConsumer_->pathPipe_.get( newPath );
        painter_.setData( newPath );
    }
    if ( pathUpdateConsumer_->indexPipe_.isNewData() )
    {
        int index;
        pathUpdateConsumer_->indexPipe_.get( index );
        painter_.setWpIndex( index );
    }
    
    // get the activation time
    bool isEnabled;
    int ret = isFollowerEnabled(isEnabled);
    if (ret!=0) return;
    
    if ( (activationTimer_->elapsedSec()>0.5) && isEnabled) 
    {
        try
        {
            double secondsSinceActivation;
            if (pathFollower2dPrx_->getRelativeActivationTime( secondsSinceActivation )) 
            {
                painter_.setRelativeStartTime( secondsSinceActivation );
            }
        }
        catch ( const orca::OrcaException &e )
        {
            stringstream ss;
            ss << e.what;
            humanManager_.showStatusError(  ss.str().c_str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "While trying to get activation time: " << endl << e;
            humanManager_.showStatusError(  ss.str().c_str() );
        }
        activationTimer_->restart();
    }
}

int 
PathFollower2dElement::isFollowerEnabled( bool &isEnabled )
{
    try
    {
        isEnabled = pathFollower2dPrx_->enabled();
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
    cout << "TRACE(pathfollower2delement.cpp): setUseTransparency: " << useTransparency << endl;
    painter_.setUseTransparency( useTransparency ); 
    if ( pathHI_.get() )
        pathHI_->setUseTransparency( useTransparency );
    currentTransparency_ = useTransparency;
}

int
PathFollower2dElement::connectToInterface()
{
    if (isConnected_) return 0;
    
    humanManager_.showStatusInformation( "PathFollowerElement is trying to connect");
    
    // Here's what IceStormElement2d usually does for you if the GuiElement inherits from IceStormElement (see comments in .h file for more information)
    try 
    {
        orcaice::connectToInterfaceWithString( context_, pathFollower2dPrx_, proxyString_ );
        Ice::ObjectPtr pathFollowerObj = pathUpdateConsumer_;
        
        // subscribe
        callbackPrx_ = orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( context_, pathFollowerObj );
        pathFollower2dPrx_->subscribe(callbackPrx_);
    }
    catch ( const IceUtil::Exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement:: Problem connecting to pathfollower interface: " << e;
        humanManager_.showStatusWarning( ss.str().c_str() );
        return -1;
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement:: Problem connecting to pathfollower interface: " << e.what();
        humanManager_.showStatusWarning( ss.str().c_str() );
        return -1;
    }
    catch ( ... )
    {
        humanManager_.showStatusWarning( "PathFollower2dElement: Unknown problem connecting to pathfollower interface.");
        return -1;
    }
    humanManager_.showStatusInformation( "Connected to pathfollower interface successfully.");
    
    isConnected_ = true;
    return 0;
}

void
PathFollower2dElement::getInitialData()
{
    try
    {        
        // get initial path and set pipe
        orca::PathFollower2dData data = pathFollower2dPrx_->getData();
        pathUpdateConsumer_->pathPipe_.set( data );
                
        // get initial waypoint in focus and set pipe
        int wpIndex = pathFollower2dPrx_->getWaypointIndex();
        pathUpdateConsumer_->indexPipe_.set( wpIndex );
    }
    catch ( Ice::Exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement::"<<__func__<<": "<< e << endl;
        // humanManager_.showStatusWarning( ss.str().c_str() );
        cout << ss.str().c_str();
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement::"<<__func__<<": "<< e.what() << endl;
        // humanManager_.showStatusWarning( ss.str().c_str() );
        cout << ss.str().c_str();
    }
    catch ( ... )
    {
        // humanManager_.showStatusWarning( "PathFollower2d: Problem getting initial data.");
        cout << "PathFollower2dElement: Unknown exception in " << __func__ << endl;
    }
}

void
PathFollower2dElement::doInitialSetup()
{
    if ( connectToInterface()==0 )
        getInitialData();
}

QStringList
PathFollower2dElement::contextMenu()
{
    QStringList s;
    if (displayWaypoints_) {
        s << "Switch all waypoints OFF";
    } else {
        s << "Switch all waypoints ON";
    }
    if (displayPastWaypoints_) {
        s << "Switch past waypoints OFF";
    } else {
        s << "Switch past waypoints ON";
    }
    if (displayFutureWaypoints_) {
        s << "Switch future waypoints OFF";
    } else {
        s << "Switch future waypoints ON";
    }
    if (displayOlympicMarker_) {
        s << "Switch olympic marker OFF";
    } else {
        s << "Switch olympic marker ON";
    }
    if (currentTransparency_) {
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

bool
PathFollower2dElement::tryEnableRemoteInterface( bool enable )
{
    try 
    {
        pathFollower2dPrx_->setEnabled( enable );
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "Error when switching remote interface to " << enable << ". Reason: " << e.what() << endl;
        humanManager_.showStatusError( QString(ss.str().c_str()) );  
        return false;
    }
    return true;
}

void 
PathFollower2dElement::execute( int action )
{
    cout<<"TRACE(pathfollower2delement.cpp): execute: " << action << endl;
    if ( action == 0 )
    {
        displayWaypoints_ = !displayWaypoints_;
        painter_.toggleDisplayWaypoints();    
    }
    else if ( action == 1 )
    {
        displayPastWaypoints_ = !displayPastWaypoints_;
        painter_.togglePastWaypoints();
    }
    else if ( action == 2 )
    {
        displayFutureWaypoints_ = !displayFutureWaypoints_;
        painter_.toggleFutureWaypoints();
    }
    else if ( action == 3 )
    {
        displayOlympicMarker_ = !displayOlympicMarker_;
        painter_.toggleOlympicMarker();
    }
    else if ( action == 4 )
    {
        setUseTransparency(!currentTransparency_);
    }
    else if ( action == 5 )
    {
        if (isRemoteInterfaceSick_) return;
        pathFollower2dPrx_->setEnabled( !pathFollower2dPrx_->enabled() );
        bool isEnabled;
        isFollowerEnabled( isEnabled );
        QString str;
        if (isEnabled) {
            str = "Pathfollower reports it is ENABLED now.";
        } else {
            str = "Pathfollower reports it is DISABLED now.";
        }
        humanManager_.showStatusInformation(str);
    }
    else if ( action == 6 )
    {
        if ( pathHI_.get() )
            pathHI_->savePathAs();
    }
    else if ( action == 7 )
    {
        if ( pathHI_.get() )
            pathHI_->savePath();
    }
    else
    {
        assert( false && "PathFollower2dElement::execute(): bad action" );
    }
}

void 
PathFollower2dElement::go()
{
    cout<<"TRACE(PathFollower2dElement): go()" << endl;
    humanManager_.showStatusInformation("Received GO signal");
    try
    {
        pathFollower2dPrx_->activateNow();
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to activate pathfollower: " << endl << e;
        humanManager_.showStatusError( QString(ss.str().c_str()) );
    }
}

void 
PathFollower2dElement::stop()
{
    cout<<"TRACE(PathFollower2dElement): stop()" << endl;
    humanManager_.showStatusInformation("Received STOP signal");
    orca::PathFollower2dData dummyPath;
    const bool activateNow = true;
    try
    {
        pathFollower2dPrx_->setData( dummyPath, activateNow );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set (empty) pathfollower data: " << endl << e;
        humanManager_.showDialogError( QString(ss.str().c_str()) );
    }
}

void 
PathFollower2dElement::sendPath( const PathFollowerInput &pathInput, bool activateImmediately )
{
    try
    {
        // it's possible that we were desubscribed before, let's resubscribe to make sure
        // if we are already subscribed, the server will just do nothing
        pathFollower2dPrx_->subscribe(callbackPrx_);

        orca::PathFollower2dData data;
        bool isOk = pathInput.getPath( data );


        if (isOk) 
        {
            cout<<"TRACE(pathfollower2delement.cpp): setData() with data: " << orcaobj::toVerboseString(data) << endl;
            pathFollower2dPrx_->setData( data, activateImmediately );
        } else {
            humanManager_.showStatusWarning( "No path to send!" );
            return;
        }
        if (!activateImmediately) 
            humanManager_.showStatusInformation( "Path needs to be activated by pressing the Go button." );
    }
    catch ( const orca::OrcaException &e )
    {
        stringstream ss;
        ss << e.what;
        humanManager_.showDialogError( ss.str().c_str() );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set pathfollowing data: " << endl << e;
        humanManager_.showStatusError(  ss.str().c_str() );
    }
    
}

void 
PathFollower2dElement::paint( QPainter *p, int z )
{   
    painter_.paint( p, z );
    if ( pathHI_.get() )
        pathHI_->paint( p );
}

}
