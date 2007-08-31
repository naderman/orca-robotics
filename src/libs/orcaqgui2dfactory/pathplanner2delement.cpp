/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "pathplanner2delement.h"
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui/guiicons.h>
#include <orcaice/orcaice.h>
#include <QFileDialog>
#include <orcaqgui2dfactory/wptolerancesdialog.h>


using namespace std;
using namespace orca;
using namespace orcaqgui;

namespace orcaqgui2d {

////////////////////////////////////////////////////////////////////////////////
void
PathPlannerTaskAnswerConsumer::setData(const ::orca::PathPlanner2dData& data, const ::Ice::Current& )
{
    std::cout << "INFO(pathplanner2dconsumerI.cpp): Received results: " << orcaice::toVerboseString(data) << std::endl;
    if (data.result==PathOk) return; 
    
    QString msg("Pathplanner could not compute path!\nReason is: ");
    
    if (data.result==PathStartNotValid) 
        msg.append("Path Start not valid");    
    else if (data.result==PathDestinationNotValid) 
        msg.append("Path Destination not valid");    
    else if (data.result==PathDestinationUnreachable) 
        msg.append("Destination unreachable");    
    else if (data.result==PathOtherError) 
        msg.append("OtherError");

    msg.append( ": " );
    msg.append( QString(data.resultDescription.c_str()) );

    msgProxy_.set(msg); 
}
////////////////////////////////////////////////////////////////////////////////

PathplannerButtons::PathplannerButtons( QObject *parent, orcaqgui::IHumanManager *humanManager, string proxyString)
{
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);

    QPixmap globalwpIcon(globalwaypoint_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

    QAction *fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathPlanner Path As", this );
    connect(fileSavePathAs, SIGNAL(activated()), parent, SLOT(savePathAs()));
    QAction *fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathPlanner Path", this );
    connect(fileSavePath, SIGNAL(activated()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(globalwpIcon, QString(proxyString.c_str()) + "\n" + "PathPlanner waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(activated()), parent, SLOT(waypointModeSelected()) );

    QAction *hiSend = new QAction(sendIcon, QString(proxyString.c_str()) + "\n" + "Send PathPlanner Task", this);
    connect( hiSend,SIGNAL(activated()), parent, SLOT(send()) );
    QAction *hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "Discard PathPlanner Task", this);
    connect( hiCancel,SIGNAL(activated()), parent ,SLOT(cancel()) );
    
    QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "PathPlanner Waypoint settings", this );
    connect( wpDialogAction, SIGNAL(activated()), parent, SLOT(waypointSettingsDialog()) );

    humanManager->fileMenu()->addAction(fileSavePathAs);
    humanManager->fileMenu()->addAction(fileSavePath);

//     humanManager->toolBar()->addAction(fileSavePathAs);
//     humanManager->toolBar()->addAction(fileSavePath);

    humanManager->toolBar()->addAction( hiWaypoints_ );
    humanManager->toolBar()->addAction( hiSend );
    humanManager->toolBar()->addAction( hiCancel );
    
    humanManager->optionsMenu()->addAction( wpDialogAction );
    
    QAction *sep = humanManager->toolBar()->addSeparator();
    sep->setParent( this );
}

void 
PathplannerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}
/////////////////////////////////////////////////////

PathPlanner2dElement::PathPlanner2dElement( const orcaice::Context & context,
                                            const std::string &proxyString,
                                            orcaqgui::IHumanManager* humanManager )
    : IceStormElement<  PathPainter,
                        orca::PathPlanner2dData,
                        orca::PathPlanner2dPrx,
                        orca::PathPlanner2dConsumer,
                        orca::PathPlanner2dConsumerPrx>( context, proxyString, painter_, -1 ),
      context_(context),
      proxyString_(proxyString),
      humanManager_(humanManager),
      displayWaypoints_(true),
      currentTransparency_(false),
      pathHI_( this,
               proxyString,
               humanManager,
               painter_,
               readWaypointSettings( context_.properties(), context_.tag() ) )
{
    cout<<"TRACE(pathplanner2delement.cpp): Instantiating w/ proxyString '" << proxyString << "'" << endl;   
    painter_.initialize( displayWaypoints_, true, currentTransparency_);
    pathTaskAnswerConsumer_ = new PathPlannerTaskAnswerConsumer;
}

PathPlanner2dElement::~PathPlanner2dElement()
{
}

void
PathPlanner2dElement::update()
{    
     if ( !needToUpdate() ) {
        return;
    }
    
    listener_.buffer().getAndPop( data_ );
    painter_.setData( data_ );
    
    if ( pathTaskAnswerConsumer_->msgProxy_.isNewData() )
    {
        QString msg;
        pathTaskAnswerConsumer_->msgProxy_.get( msg );
        humanManager_->showBoxMsg(Error, msg);    
    }
}

void 
PathPlanner2dElement::setTransparency( bool useTransparency ) 
{ 
    cout << "TRACE(pathplanner2delement.cpp): setTransparency: " << useTransparency << endl;
    painter_.setTransparency( useTransparency ); 
    pathHI_.setTransparency( useTransparency );
    currentTransparency_ = useTransparency;
}

void
PathPlanner2dElement::actionOnConnection()
{
    humanManager_->showStatusMsg(Information, "PathplannerElement is trying to connect");
     
    try 
    {
        orcaice::connectToInterfaceWithString( context_, pathPlanner2dPrx_, proxyString_ );
    }
    catch ( ... )
    {
        humanManager_->showStatusMsg(Warning, "Problem connecting to pathplanner interface. Will try again later.");
        //cout << "WARNING(pathplanner2delement.cpp): Problem connecting to interface. Will try again later." << endl;
        return;
    }
    humanManager_->showStatusMsg(Information, "Connected to pathplanner interface successfully.");
    
    pathPlanner2dConsumerObj_ = pathTaskAnswerConsumer_;
    taskCallbackPrx_ = orcaice::createConsumerInterface<PathPlanner2dConsumerPrx>( context_, pathPlanner2dConsumerObj_ );
}

QStringList
PathPlanner2dElement::contextMenu()
{
    QStringList s;
    if (displayWaypoints_) {
        s << "Switch all waypoints OFF";
    } else {
        s << "Switch all waypoints ON";
    }
    if (currentTransparency_) {
        s << "Switch transparency OFF";
    } else {
        s << "Switch transparency ON";
    }
    
    s  << "Save path as..."
       << "Save path";
    return s;
}

void 
PathPlanner2dElement::execute( int action )
{
    cout<<"TRACE(pathplanner2delement.cpp): execute: " << action << endl;
    if ( action == 0 )
    {
        displayWaypoints_ = !displayWaypoints_;
        painter_.toggleDisplayWaypoints();    
    }  
    else if ( action == 1 )
    {
        setTransparency(!currentTransparency_);
    }
    else if ( action == 2 )
    {
        pathHI_.savePathAs();
    }
    else if ( action == 3 )
    {
        pathHI_.savePath();
    }
    else
    {
        assert( false && "PathPlanner2dElement::execute(): bad action" );
    }
}

void 
PathPlanner2dElement::sendPath( const PathPlannerInput &pathInput )
{
    try
    {
        PathPlanner2dTask task = pathInput.getTask();
        task.prx = taskCallbackPrx_;
        pathPlanner2dPrx_->setTask( task );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set a pathfollowing data: " << endl << e;
        humanManager_->showStatusMsg( Error, ss.str().c_str() );
    }
}

void 
PathPlanner2dElement::paint( QPainter *p, int z )
{ 
    painter_.paint( p, z );
    pathHI_.paint( p );
}

void
PathPlanner2dElement::setFocus( bool inFocus ) 
{ 
    painter_.setFocus( inFocus );
    pathHI_.setFocus( inFocus );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

PathPlannerHI::PathPlannerHI( PathPlanner2dElement *ppElement,
                              string proxyString,
                              orcaqgui::IHumanManager *humanManager, 
                              PathPainter &painter,
                              WaypointSettings wpSettings )
    : ppElement_(ppElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      painter_(painter),
      pathFileName_("/home"),
      pathFileSet_(false),
      wpSettings_(wpSettings),
      pathInput_(NULL),
      gotMode_(false)
{
    buttons_ = new PathplannerButtons( this, humanManager, proxyString );
}

PathPlannerHI::~PathPlannerHI()
{
    if ( pathInput_ ) delete pathInput_;
    if ( buttons_ ) delete buttons_;
}

void 
PathPlannerHI::setFocus( bool inFocus )
{
    if (inFocus) {
        if (buttons_==0) {
            buttons_ = new PathplannerButtons( this, humanManager_, proxyString_ );
        }
    } else {
        delete buttons_;
        buttons_=0;
    }
}

void 
PathPlannerHI::paint( QPainter *p )
{
    if ( pathInput_ ) 
    {
        pathInput_->paint(p);
    }
}

void PathPlannerHI::waypointSettingsDialog()
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

    if (pathInput_!=NULL)
        pathInput_->updateWpSettings( &wpSettings_ );
}

void 
PathPlannerHI::waypointModeSelected()
{
    if ( gotMode_ ) return;
    gotMode_ = humanManager_->requestMode( ppElement_ );

    if ( !gotMode_ )
    {
        humanManager_->showBoxMsg( Warning, "Couldn't take over the mode for PathPlanner waypoints!" );
        return;
    }

    pathInput_ = new PathPlannerInput( this, &wpSettings_, humanManager_ );
    pathInput_->setTransparency( useTransparency_ );
    buttons_->setWpButton( true );   
}

void
PathPlannerHI::setTransparency( bool useTransparency )
{ 
    useTransparency_ = useTransparency;
    if (pathInput_) 
        pathInput_->setTransparency( useTransparency ); 
}

void 
PathPlannerHI::send()
{
    if ( pathInput_ != NULL )
        ppElement_->sendPath( *pathInput_ );
    cancel();
}
void 
PathPlannerHI::cancel()
{
    if ( gotMode_ )
    {
        humanManager_->relinquishMode( ppElement_ );
        lostMode();
    }
}
void
PathPlannerHI::lostMode()
{
    assert( pathInput_ != NULL );
    delete pathInput_;
    pathInput_ = NULL;
    buttons_->setWpButton( false );
    gotMode_ = false;
}

void 
PathPlannerHI::savePathAs()
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
PathPlannerHI::savePath()
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

