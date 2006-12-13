/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <QFileDialog>

#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui/guiicons.h>
#include <orcaqgui2dfactory/wptolerancesdialog.h>

#include "pathfollower2delement.h"

using namespace std;
using namespace orca;

namespace orcaqgui {

////////////////////////////////////////////////////////////////////////////////

void
PathUpdateConsumer::setData( const orca::PathFollower2dDataPtr &newPath, const ::Ice::Current& )
{
    pathPipe_.set( newPath );
}

void
PathUpdateConsumer::setWaypointIndex( int index, const ::Ice::Current& )
{
    indexPipe_.set( index );
}

////////////////////////////////////////////////////////////////////////////////

PathfollowerButtons::PathfollowerButtons( QObject *parent, IHumanManager *humanManager, string proxyString)
{
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);

    QPixmap waypointsIcon(waypoints_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

    QAction* fileOpenPath = new QAction(openIcon, QString(proxyString.c_str()) + "\n" + "Open PathFollower Path File", this );
    connect(fileOpenPath, SIGNAL(activated()), parent, SLOT(loadPathFromFile()));
    QAction* fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path As", this );
    connect(fileSavePathAs, SIGNAL(activated()), parent, SLOT(savePathAs()));
    QAction* fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path", this );
    connect(fileSavePath, SIGNAL(activated()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(waypointsIcon, QString(proxyString.c_str()) + "\n" + "&Pathfollower waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(activated()), parent, SLOT(waypointModeSelected()) );

    QAction* hiSend = new QAction(sendIcon,QString(proxyString.c_str()) + "\n" + "&Send PathFollower Path", this);
    connect( hiSend,SIGNAL(activated()), parent, SLOT(send()) );
    QAction* hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "&Discard PathFollower Path", this);
    connect( hiCancel,SIGNAL(activated()), parent ,SLOT(cancel()) );

    QAction* hiGo = new QAction(goIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Go All Robots", this);
    connect( hiGo, SIGNAL(activated()), parent, SLOT(allGo()) ); 
    QAction* hiStop = new QAction(stopIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Stop All Robots", this);
    hiStop->setShortcut(QKeySequence(Qt::Key_Escape));
    connect( hiStop, SIGNAL(activated()), parent, SLOT(allStop()) ); 

    humanManager->fileMenu()->addAction(fileOpenPath);
    humanManager->fileMenu()->addAction(fileSavePathAs);
    humanManager->fileMenu()->addAction(fileSavePath);

    humanManager->toolBar()->addAction(fileOpenPath);
    humanManager->toolBar()->addAction(fileSavePathAs);
    humanManager->toolBar()->addAction(fileSavePath);

    humanManager->toolBar()->addAction( hiWaypoints_ );
    humanManager->toolBar()->addAction( hiSend );
    humanManager->toolBar()->addAction( hiCancel );
    humanManager->toolBar()->addAction( hiGo );
    humanManager->toolBar()->addAction( hiStop );

    QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "&PathFollower Waypoint settings", this );
    connect( wpDialogAction, SIGNAL(activated()), parent, SLOT(waypointSettingsDialog()) );
    humanManager->optionsMenu()->addAction( wpDialogAction );

    QAction *sep = humanManager->toolBar()->addSeparator();
    sep->setParent( this );
}

void 
PathfollowerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}
/////////////////////////////////////////////////////

PathFollower2dElement::PathFollower2dElement( orcaice::Context   context,
                                              const std::string &proxyString,
                                              IHumanManager *humanManager)
    : doneInitialSetup_(false),
      proxyString_(proxyString),
      context_(context),
      humanManager_(humanManager),
      firstTime_(true),
      numPathDumps_(0),
      pathHI_( this,
               proxyString,
               humanManager,
               painter_,
               readWaypointSettings( context ),
               readActivateImmediately( context ))
{
    cout<<"TRACE(pathfollower2delement.cpp): Instantiating w/ proxyString '" << proxyString << "'" << endl;
    pathUpdateConsumer_ = new PathUpdateConsumer;
    
    getDumpPath();

    timer_ = new orcaice::Timer;
}

PathFollower2dElement::~PathFollower2dElement()
{
}

void
PathFollower2dElement::getDumpPath()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.";
    Ice::StringSeq strIn; strIn.push_back("/tmp"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringSeqWithDefault( prop, prefix+"DumpPath", strIn );
    dumpPath_ = QString(strOut[0].c_str());
}

void
PathFollower2dElement::update()
{
    // if initial setup is not done yet (no connection established)
    // then try to connect every 5 seconds
    if ( !doneInitialSetup_ )
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
    
    if ( pathUpdateConsumer_->pathPipe_.isNewData() )
    {
        orca::PathFollower2dDataPtr newPath;
        pathUpdateConsumer_->pathPipe_.get( newPath );
        painter_.setData( newPath );
    }
    if ( pathUpdateConsumer_->indexPipe_.isNewData() )
    {
        int index;
        pathUpdateConsumer_->indexPipe_.get( index );
        painter_.setWpIndex( index );
    }
}

void
PathFollower2dElement::doInitialSetup()
{
    humanManager_->showStatusMsg(Information, "PathFollowerElement is trying to connect");
    
    // Here's what IceStormElement usually does for you if the GuiElement inherits from IceStormElement (see comments in .h file for more information)
    try 
    {
        orcaice::connectToInterfaceWithString( context_, pathFollower2dPrx_, proxyString_ );
        Ice::ObjectPtr pathFollowerObj = pathUpdateConsumer_;
        orca::PathFollower2dConsumerPrx callbackPrx = 
                orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( context_, pathFollowerObj );
        pathFollower2dPrx_->subscribe(callbackPrx);
    }
    catch ( ... )
    {
        humanManager_->showStatusMsg(Warning, "Problem connecting to pathfollower interface. Will try again later.");
        return;
    }
    humanManager_->showStatusMsg(Information, "Connected to pathfollower interface successfully.");
    
    doneInitialSetup_ = true;
}

QStringList
PathFollower2dElement::contextMenu()
{
    QStringList s;
    s << "Toggle All Waypoints" << "Toggle Past Waypoints" << "Toggle enabled";
    return s;
}

void 
PathFollower2dElement::execute( int action )
{
    cout<<"TRACE(pathfollower2delement.cpp): execute: " << action << endl;
    if ( action == 0 )
    {
        painter_.toggleDisplayWaypoints();    
    }
    else if ( action == 1 )
    {
        painter_.togglePastWaypoints();
    }
    else if ( action == 2 )
    {
        pathFollower2dPrx_->setEnabled( !pathFollower2dPrx_->enabled() );
    }
    else
    {
        assert( false && "PathFollower2dElement::execute(): bad action" );
    }
}

void 
PathFollower2dElement::go()
{
    cout<<"TRACE(pathfollower2delement.cpp): go()" << endl;
    try
    {
        pathFollower2dPrx_->activateNow();
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to activate pathfollower: " << endl << e;
        humanManager_->showStatusMsg(Error,QString(ss.str().c_str()));
    }
}

void 
PathFollower2dElement::stop()
{
    cout<<"TRACE(pathfollower2delement.cpp): stop()" << endl;
    PathFollower2dDataPtr dummyPath = new PathFollower2dData;
    const bool activateNow = false;
    try
    {
        pathFollower2dPrx_->setData( dummyPath, activateNow );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set (empty) pathfollower data: " << endl << e;
        humanManager_->showBoxMsg(Error,QString(ss.str().c_str()));
    }
}

void 
PathFollower2dElement::sendPath( const PathFollowerInput &pathInput, bool activateImmediately )
{
    try
    {
        pathFollower2dPrx_->setData( pathInput.getPath(), activateImmediately );
        if (!activateImmediately) 
            humanManager_->showStatusMsg( Information, "Path needs to be activated by pressing the Go buttion." );
        
        // save path to file automatically
        char buffer [5];
        sprintf(buffer,"%05d",numPathDumps_++);
        QString filename = dumpPath_ + "/pathdump" + QString(buffer) + ".txt";
        pathHI_.savePath( filename, humanManager_ );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set a pathfollowing data: " << endl << e;
        humanManager_->showStatusMsg( Error, ss.str().c_str() );
    }
}

void 
PathFollower2dElement::paint( QPainter *p, int z )
{ 
    painter_.paint( p, z );
    pathHI_.paint( p );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

PathFollowerHI::PathFollowerHI( PathFollower2dElement *pfElement,
                                string proxyString,
                                IHumanManager *humanManager, 
                                PathPainter &painter,
                                WaypointSettings wpSettings,
                                bool activateImmediately )
    : pfElement_(pfElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      painter_(painter),
      pathFileName_("/home"),
      pathFileSet_(false),
      wpSettings_(wpSettings),
      activateImmediately_(activateImmediately),
      pathInput_(NULL),
      gotMode_(false)
{
    buttons_ = new PathfollowerButtons( this, humanManager, proxyString );
}

PathFollowerHI::~PathFollowerHI()
{
    if ( pathInput_ ) delete pathInput_;
    if ( buttons_ ) delete buttons_;
}

void 
PathFollowerHI::setFocus( bool inFocus )
{
    if (inFocus) {
        if (buttons_==0) {
            buttons_ = new PathfollowerButtons( this, humanManager_, proxyString_ );
        }
    } else {
        delete buttons_;
        buttons_=0;
    }
}

void 
PathFollowerHI::paint( QPainter *p )
{
    if ( pathInput_ ) 
    {
        pathInput_->paint(p);
    }
}

void PathFollowerHI::waypointSettingsDialog()
{
    QDialog *myDialog = new QDialog;
    Ui::WpTolerancesDialog ui;
    ui.setupUi(myDialog);
    
    ui.timePeriodSpin->setValue( wpSettings_.timePeriod );
    ui.distanceSpin->setValue( wpSettings_.distanceTolerance );
    ui.headingSpin->setValue( wpSettings_.headingTolerance );
    ui.maxSpeedSpin->setValue( wpSettings_.maxApproachSpeed );
    ui.maxTurnrateSpin->setValue( wpSettings_.maxApproachTurnrate );
    
    int ret = myDialog->exec();
    if (ret==QDialog::Rejected) return;
    
    wpSettings_.timePeriod = ui.timePeriodSpin->value();
    wpSettings_.distanceTolerance = ui.distanceSpin->value(); 
    wpSettings_.headingTolerance = ui.headingSpin->value();
    wpSettings_.maxApproachSpeed = ui.maxSpeedSpin->value();
    wpSettings_.maxApproachTurnrate = ui.maxTurnrateSpin->value();

    if (pathInput_!=NULL)
        pathInput_->updateWpSettings( &wpSettings_ );
}

void 
PathFollowerHI::waypointModeSelected()
{
    if ( gotMode_ ) return;
    gotMode_ = humanManager_->requestMode( pfElement_ );
    
    if ( !gotMode_ )
    {
        humanManager_->showBoxMsg( Warning, "Couldn't take over the mode for PathFollower waypoints!" );
        return;
    }

    pathInput_ = new PathFollowerInput( &wpSettings_ );
    buttons_->setWpButton( true );    
}
void 
PathFollowerHI::send()
{
    pfElement_->sendPath( *pathInput_, activateImmediately_ );
    cancel();
}
void 
PathFollowerHI::cancel()
{
    if ( gotMode_ )
    {
        humanManager_->relinquishMode( pfElement_ );
        lostMode();
    }
}
void 
PathFollowerHI::allGo()
{
    const QList<GuiElement*> elements = humanManager_->guiElementModel().elements();
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
    const QList<GuiElement*> elements = humanManager_->guiElementModel().elements();
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
PathFollowerHI::lostMode()
{
    assert( pathInput_ != NULL );
    delete pathInput_;
    pathInput_ = NULL;
    buttons_->setWpButton( false );
    gotMode_ = false;
}

void 
PathFollowerHI::loadPathFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(
            0,
            "Choose a path file to open", 
            pathFileName_,
            "*.txt");

    if (!fileName.isEmpty())
    {
        waypointModeSelected();
        if ( !gotMode_ )
            return;
        pathInput_->loadDataFromFile( &fileName );
        pathFileName_ = fileName;
        pathFileSet_ = true;
    }
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
        painter_.savePath( fileName, humanManager_ );
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
        painter_.savePath( pathFileName_, humanManager_ );
    }
}



}
