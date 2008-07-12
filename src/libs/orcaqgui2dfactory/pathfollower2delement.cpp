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
#include <QFileDialog>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaobj/stringutils.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui/guiicons.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui2dfactory/wptolerancesdialog.h>

#include "pathfollower2delement.h"

using namespace std;

namespace orcaqgui2d {

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

PathfollowerButtons::PathfollowerButtons( QObject                       *parent,
                                          hydroqguielementutil::IHumanManager      &humanManager,
                                          hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                          string                         proxyString)
    : humanManager_(humanManager),
      shortcutKeyManager_(shortcutKeyManager)
{
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);

    QPixmap waypointsIcon(waypoints_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

    QAction* fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path As", this );
    connect(fileSavePathAs, SIGNAL(triggered()), parent, SLOT(savePathAs()));
    QAction* fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path", this );
    connect(fileSavePath, SIGNAL(triggered()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(waypointsIcon, QString(proxyString.c_str()) + "\n" + "&Pathfollower waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(triggered()), parent, SLOT(waypointModeSelected()) );

    QAction* hiSend = new QAction(sendIcon,QString(proxyString.c_str()) + "\n" + "&Send PathFollower Path", this);
    connect( hiSend,SIGNAL(triggered()), parent, SLOT(send()) );
    QAction* hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "&Discard PathFollower Path", this);
    connect( hiCancel,SIGNAL(triggered()), parent ,SLOT(cancel()) );

    QAction* hiGo = new QAction(goIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Go All Robots", this);
    connect( hiGo, SIGNAL(triggered()), parent, SLOT(allGo()) ); 
    QAction* hiStop = new QAction(stopIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Stop All Robots", this);
    connect( hiStop, SIGNAL(triggered()), parent, SLOT(allStop()) ); 

    humanManager.fileMenu()->addAction(fileSavePathAs);
    humanManager.fileMenu()->addAction(fileSavePath);

    shortcutKeyManager.subscribeToShortcutKey( hiStop, QKeySequence(Qt::Key_Escape), false, this );
    humanManager.toolBar()->addAction(hiStop);
    shortcutKeyManager.subscribeToShortcutKey( hiWaypoints_, QKeySequence(Qt::Key_F10), true, this );
    humanManager.toolBar()->addAction(hiWaypoints_);
    shortcutKeyManager.subscribeToShortcutKey( hiSend, QKeySequence(Qt::Key_F11), true, this );
    humanManager.toolBar()->addAction(hiSend);
    shortcutKeyManager.subscribeToShortcutKey( hiGo, QKeySequence(Qt::Key_F12), false, this );
    humanManager.toolBar()->addAction(hiGo);
    humanManager.toolBar()->addAction( hiCancel );

    QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "&PathFollower Waypoint settings", this );
    connect( wpDialogAction, SIGNAL(triggered()), parent, SLOT(waypointSettingsDialog()) );
    humanManager.optionsMenu()->addAction( wpDialogAction );

    QAction *sep = humanManager.toolBar()->addSeparator();
    sep->setParent( this );
}

PathfollowerButtons::~PathfollowerButtons() 
{
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_Escape), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F10), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F11), this ); 
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_F12), this ); 
}

void 
PathfollowerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}
/////////////////////////////////////////////////////

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
    pathHI_.reset( new PathFollowerHI( this,
                                       proxyString_,
                                       humanManager_,
                                       mouseEventManager_,
                                       shortcutKeyManager_,
                                       guiElementSet_,
                                       painter_,
                                       readWaypointSettings( context_.properties(), context_.tag() ),
                                       readActivateImmediately( context_.properties(), context_.tag() ),
                                       readDumpPath( context_.properties(), context_.tag() ) ) );
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
            humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Error, ss.str().c_str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "While trying to get activation time: " << endl << e;
            humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Error, ss.str().c_str() );
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
            humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Information, "PathFollower2dElement::remote interface is sane again" );
            isRemoteInterfaceSick_ = false;
        }
        return 0;
    }
    catch ( ... )
    {
        if (!isRemoteInterfaceSick_) {
            humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Error, "PathFollower2dElement::remote interface seems sick" );
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
    
    humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information, "PathFollowerElement is trying to connect");
    
    // Here's what IceStormElement usually does for you if the GuiElement inherits from IceStormElement (see comments in .h file for more information)
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
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, ss.str().c_str() );
        return -1;
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement:: Problem connecting to pathfollower interface: " << e.what();
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, ss.str().c_str() );
        return -1;
    }
    catch ( ... )
    {
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "PathFollower2dElement: Unknown problem connecting to pathfollower interface.");
        return -1;
    }
    humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Connected to pathfollower interface successfully.");
    
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
        // humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, ss.str().c_str() );
        cout << ss.str().c_str();
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "PathFollower2dElement::"<<__func__<<": "<< e.what() << endl;
        // humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, ss.str().c_str() );
        cout << ss.str().c_str();
    }
    catch ( ... )
    {
        // humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Warning, "PathFollower2d: Problem getting initial data.");
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
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Error, QString(ss.str().c_str()) );  
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
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information,str);
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
    humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information,"Received GO signal");
    try
    {
        pathFollower2dPrx_->activateNow();
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to activate pathfollower: " << endl << e;
        humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Error,QString(ss.str().c_str()));
    }
}

void 
PathFollower2dElement::stop()
{
    cout<<"TRACE(PathFollower2dElement): stop()" << endl;
    humanManager_.showStatusMsg(hydroqguielementutil::IHumanManager::Information,"Received STOP signal");
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
        humanManager_.showDialogMsg(hydroqguielementutil::IHumanManager::Error,QString(ss.str().c_str()));
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
            humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Warning, "No path to send!" );
            return;
        }
        if (!activateImmediately) 
            humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Information, "Path needs to be activated by pressing the Go button." );
    }
    catch ( const orca::OrcaException &e )
    {
        stringstream ss;
        ss << e.what;
        humanManager_.showDialogMsg( hydroqguielementutil::IHumanManager::Error, ss.str().c_str() );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set pathfollowing data: " << endl << e;
        humanManager_.showStatusMsg( hydroqguielementutil::IHumanManager::Error, ss.str().c_str() );
    }
    
}

void 
PathFollower2dElement::paint( QPainter *p, int z )
{   
    painter_.paint( p, z );
    if ( pathHI_.get() )
        pathHI_->paint( p );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

PathFollowerHI::PathFollowerHI( PathFollower2dElement *pfElement,
                                string proxyString,
                                hydroqguielementutil::IHumanManager &humanManager, 
                                hydroqguielementutil::MouseEventManager &mouseEventManager, 
                                hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager, 
                                const hydroqgui::GuiElementSet &guiElementSet, 
                                PathPainter &painter,
                                WaypointSettings wpSettings,
                                bool activateImmediately,
                                QString dumpPath )
    : pfElement_(pfElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      shortcutKeyManager_(shortcutKeyManager),
      guiElementSet_(guiElementSet),
      painter_(painter),
      pathFileName_("/tmp"),
      pathFileSet_(false),
      wpSettings_(wpSettings),
      activateImmediately_(activateImmediately),
      gotMode_(false),
      dumpPath_(dumpPath),
      numPathDumps_(0),
      lastSavedPathFile_("")
{
    buttons_.reset( new PathfollowerButtons( this, humanManager, shortcutKeyManager, proxyString ) );
}

void 
PathFollowerHI::setFocus( bool inFocus )
{
    if ( inFocus && !buttons_.get() ) 
    {
        buttons_.reset( new PathfollowerButtons( this, humanManager_, shortcutKeyManager_, proxyString_ ) );
    } 
    else 
    {
        buttons_.reset(0);
    }
}

void 
PathFollowerHI::paint( QPainter *p )
{
    if ( pathInput_.get() ) 
        pathInput_->paint(p);
}

void PathFollowerHI::waypointSettingsDialog()
{
    QDialog *myDialog = new QDialog;
    Ui::WpTolerancesDialog ui;
    ui.setupUi(myDialog);
    
    int index = ui.spacingPropertyCombo->findText( QString(wpSettings_.spacingProperty.c_str()) );
    ui.spacingPropertyCombo->setCurrentIndex( index );
    ui.spacingValueSpin->setValue( wpSettings_.spacingValue );
    ui.distanceSpin->setValue( wpSettings_.distanceTolerance );
    ui.headingSpin->setValue( wpSettings_.headingTolerance );
    ui.maxSpeedSpin->setValue( wpSettings_.maxApproachSpeed );
    ui.maxTurnrateSpin->setValue( wpSettings_.maxApproachTurnrate );
    
    int ret = myDialog->exec();
    if (ret==QDialog::Rejected) return;
    
    wpSettings_.spacingProperty = ui.spacingPropertyCombo->currentText().toStdString();
    wpSettings_.spacingValue = ui.spacingValueSpin->value();
    wpSettings_.distanceTolerance = ui.distanceSpin->value(); 
    wpSettings_.headingTolerance = ui.headingSpin->value();
    wpSettings_.maxApproachSpeed = ui.maxSpeedSpin->value();
    wpSettings_.maxApproachTurnrate = ui.maxTurnrateSpin->value();

    if ( pathInput_.get() )
        pathInput_->updateWpSettings( &wpSettings_ );
}

void 
PathFollowerHI::waypointModeSelected()
{
    if ( gotMode_ ) return;
    gotMode_ = mouseEventManager_.requestBecomeMouseEventReceiver( pfElement_ );
    
    if ( !gotMode_ )
    {
        humanManager_.showDialogMsg( hydroqguielementutil::IHumanManager::Warning, "Couldn't take over the mode for PathFollower waypoints!" );
        return;
    }

    pathInput_.reset( new PathFollowerInput( this, &wpSettings_, humanManager_, lastSavedPathFile_ ) );
    pathInput_->setUseTransparency( useTransparency_ );
    buttons_->setWpButton( true );
}

void
PathFollowerHI::setUseTransparency( bool useTransparency )
{ 
    useTransparency_ = useTransparency;
    if ( pathInput_.get() )
        pathInput_->setUseTransparency( useTransparency ); 
}

void 
PathFollowerHI::send()
{
    cout<<"TRACE(PathFollowerHI): send()" << endl;
    
    if ( !pathInput_.get() ) 
    {
        humanManager_.showDialogMsg( hydroqguielementutil::IHumanManager::Warning, "Not in path input mode!" );
        return;
    }
        
    // save path to file automatically
    char buffer [5];
    sprintf(buffer,"%05d",numPathDumps_++);
    QString filename = dumpPath_ + "/pathdump" + QString(buffer) + ".txt";
    pathInput_->savePath( filename );
    lastSavedPathFile_ = filename;
    
    pfElement_->sendPath( *pathInput_, activateImmediately_ );
    
    cancel();
}

void 
PathFollowerHI::cancel()
{
    cout<<"TRACE(PathFollowerHI): cancel()" << endl;
    if ( gotMode_ )
    {
        mouseEventManager_.relinquishMouseEventReceiver( pfElement_ );
        noLongerMouseEventReceiver();
    }
}

void 
PathFollowerHI::go()
{
    cout<<"TRACE(PathFollowerHI): go()" << endl;
    pfElement_->go();
}

void 
PathFollowerHI::stop()
{
    cout<<"TRACE(PathFollowerHI): stop()" << endl;
    pfElement_->stop();
}

void 
PathFollowerHI::allGo()
{
    cout<<"TRACE(PathFollowerHI): allGo()" << endl;
    const QList<hydroqguielementutil::IGuiElement*> elements = guiElementSet_.elements();
    for ( int i=0; i < elements.size(); i++ )
    {
        PathFollower2dElement *typedElem = dynamic_cast<PathFollower2dElement *>(elements[i]);
        if ( typedElem != NULL )
        {
            typedElem->go();
        }
    }
}
void 
PathFollowerHI::allStop()
{
    cout<<"TRACE(PathFollowerHI): allStop()" << endl;
    const QList<hydroqguielementutil::IGuiElement*> elements = guiElementSet_.elements();
    for ( int i=0; i < elements.size(); i++ )
    {
        PathFollower2dElement *typedElem = dynamic_cast<PathFollower2dElement *>(elements[i]);
        if ( typedElem != NULL )
        {
            typedElem->stop();
        }
    }
}

void
PathFollowerHI::noLongerMouseEventReceiver()
{
    pathInput_.reset(0);
    buttons_->setWpButton( false );
    gotMode_ = false;
}

void 
PathFollowerHI::savePathAs()
{
    QString fileName = QFileDialog::getSaveFileName(
            0,
            "Choose a filename to save under",
            pathFileName_,
            "*.txt");
    
    if (!fileName.isEmpty())
    {
        painter_.savePath( fileName, &humanManager_ );
        pathFileName_ = fileName;
        pathFileSet_ = true;
    }
}

void 
PathFollowerHI::savePath()
{
    if (!pathFileSet_)
    {   
        savePathAs();
    }
    else
    {
        painter_.savePath( pathFileName_, &humanManager_ );
    }
}



}
