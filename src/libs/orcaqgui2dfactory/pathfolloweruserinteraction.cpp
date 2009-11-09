/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <QFileDialog>
#include <hydroqguielementutil/mouseeventmanager.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqgui/guielementset.h>
#include <hydroqguipath/wptolerancesdialog.h>
#include <orcaqgui2dfactory/pathconversionutil.h>
#include <orcaqgui2dfactory/pathfollower2delement.h>

#include "pathfolloweruserinteraction.h"

using namespace std;

namespace orcaqgui2d {

PathFollowerUserInteraction::PathFollowerUserInteraction( PathFollower2dElement                         *pfElement,
                                                          const std::string                             &proxyString,
                                                          hydroqguielementutil::IHumanManager           &humanManager,
                                                          hydroqguielementutil::MouseEventManager       &mouseEventManager,
                                                          hydroqguielementutil::ShortcutKeyManager      &shortcutKeyManager,
                                                          const hydroqgui::GuiElementSet                &guiElementSet,
                                                          const PathFollowerPainter                     &painter,
                                                          const orcaice::Context                        &context,
                                                          const hydroqguipath::PathInputFactory         *inputFactory,
                                                          bool                                           enableToolbarButtons )
    : pfElement_(pfElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      shortcutKeyManager_(shortcutKeyManager),
      guiElementSet_(guiElementSet),
      painter_(painter),
      context_(context),
      inputFactory_(inputFactory),
      ifacePathFileName_("/tmp"),
      haveIfacePathFileName_(false),
      userPathFileName_("/tmp"),
      haveUserPathFileName_(false),    
      numAutoPathDumps_(0),
      ownMouseEvents_(false),
      enableToolbarButtons_(enableToolbarButtons)
{
    wpSettings_ = readWaypointSettings( context.properties(), context.tag() );

    if (enableToolbarButtons_)
    {
        buttons_.reset( new hydroqguipath::PathfollowerButtons( this,
                                                                humanManager,
                                                                shortcutKeyManager,
                                                                proxyString,
                                                                !readActivateImmediately(context.properties(), context.tag())) );
    }
    ifacePathFileHandler_.reset( new PathFileHandler( humanManager ) );
    userPathFileHandler_.reset( new PathFileHandler( humanManager ) );
}

PathFollowerUserInteraction::~PathFollowerUserInteraction()
{
    if ( ownMouseEvents_ && pfElement_!=0 )
        mouseEventManager_.relinquishMouseEventReceiver( pfElement_ );
}

void 
PathFollowerUserInteraction::setFocus( bool inFocus )
{
    if (!enableToolbarButtons_) return;

    if ( inFocus )
    {
        if ( !buttons_.get() ) 
        {
            buttons_.reset( new hydroqguipath::PathfollowerButtons( this, 
                                                                    humanManager_, 
                                                                    shortcutKeyManager_, 
                                                                    proxyString_,
                                                                    !readActivateImmediately(context_.properties(), context_.tag()) ) );
        }
    } 
    else 
    {
        buttons_.reset(0);
    }
}

void 
PathFollowerUserInteraction::paint( QPainter *p )
{
    if ( pathInput_.get() ) 
        pathInput_->paint(p);
}

void PathFollowerUserInteraction::waypointSettingsDialog()
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
PathFollowerUserInteraction::waypointModeToggled(bool checked)
{
    
    if (checked)    // user wants to get into waypoint mode
    {
        ownMouseEvents_ = mouseEventManager_.requestBecomeMouseEventReceiver( pfElement_ );

        if ( !ownMouseEvents_ )
        {
            humanManager_.showStatusWarning( "Couldn't take over the mouse for PathFollower waypoints!" );
            // uncheck the button for user feedback
            if ( buttons_.get()!=0 )
                buttons_->setWpButton( false );
            return;
        }

        // if we don't have a pathInput yet, instantiate one
        if (!pathInput_.get())
        {
            pathInput_ = inputFactory_->createPathInput( this, &wpSettings_, humanManager_);
            pathInput_->setUseTransparency( useTransparency_ );
        }

    }
    else    // user turned off waypoint mode
    {
        if (ownMouseEvents_)
        {
            // let others get the mouse events, e.g. for zooming and paning
            mouseEventManager_.relinquishMouseEventReceiver( pfElement_ );
            ownMouseEvents_ = false;
        }

    }
}

void
PathFollowerUserInteraction::mousePressEvent(QMouseEvent *e)
{
    if ( pathInput_.get() )
        pathInput_->processPressEvent(e);
}

void
PathFollowerUserInteraction::mouseMoveEvent(QMouseEvent *e)
{
    if ( pathInput_.get() )
        pathInput_->processMoveEvent(e);
}

void
PathFollowerUserInteraction::mouseReleaseEvent(QMouseEvent *e)
{
    if ( pathInput_.get() )
        pathInput_->processReleaseEvent(e);
}

void
PathFollowerUserInteraction::mouseDoubleClickEvent(QMouseEvent *e)
{
    if ( pathInput_.get() )
        pathInput_->processDoubleClickEvent(e);
}
  
    
void
PathFollowerUserInteraction::setUseTransparency( bool useTransparency )
{ 
    useTransparency_ = useTransparency;
    if ( pathInput_.get() )
        pathInput_->setUseTransparency( useTransparency ); 
}

void 
PathFollowerUserInteraction::send()
{
    if ( !pathInput_.get() ) 
    {
        humanManager_.showDialogWarning( "Not in path input mode!" );
        return;
    }
        
    // save path to file automatically
    char buffer [6];
    sprintf(buffer,"%05d",numAutoPathDumps_++);
    QString fileName = readDumpPath(context_.properties(), context_.tag()) + "/pathdump" + QString(buffer) + ".txt";
    hydroqguipath::GuiPath guiPath;
    int numLoops = 0;
    float timeOffset = 0.0;
    pathInput_->getPath( guiPath, numLoops, timeOffset );
    userPathFileHandler_->savePath( fileName, guiPath, numLoops, timeOffset );
    
    pfElement_->sendPath( pathInput_.get(), readActivateImmediately(context_.properties(), context_.tag()) );
    
    cancel();
}

void 
PathFollowerUserInteraction::cancel()
{
    noLongerMouseEventReceiver();
}

void 
PathFollowerUserInteraction::go()
{
    pfElement_->go();
}

void 
PathFollowerUserInteraction::stop()
{
    pfElement_->stop();
}

void 
PathFollowerUserInteraction::allGo()
{
    const QList<hydroqguielementutil::GuiElement*> elements = guiElementSet_.elements();
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
PathFollowerUserInteraction::allStop()
{
    const QList<hydroqguielementutil::GuiElement*> elements = guiElementSet_.elements();
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
PathFollowerUserInteraction::toggleEnabled()
{
    pfElement_->toggleEnabled();
}

void
PathFollowerUserInteraction::noLongerMouseEventReceiver()
{
    pathInput_.reset(0);
    if ( buttons_.get()!=0 )
        buttons_->setWpButton( false );
    
    if ( ownMouseEvents_ )
    {
        mouseEventManager_.relinquishMouseEventReceiver( pfElement_ );
        ownMouseEvents_ = false;
    }
}

void 
PathFollowerUserInteraction::savePathAs()
{   
    QString fileName = QFileDialog::getSaveFileName(
            0,
            "Choose a filename to save under",
            ifacePathFileName_,
            "*.txt");
    
    if (!fileName.isEmpty())
    {
        ifacePathFileHandler_->savePath( fileName, painter_.currentPath() );
        ifacePathFileName_ = fileName;
        haveIfacePathFileName_ = true;
    }
}

void 
PathFollowerUserInteraction::savePath()
{
    if (!haveIfacePathFileName_) 
        savePathAs();
    else
        ifacePathFileHandler_->savePath( ifacePathFileName_, painter_.currentPath() );
}

void
PathFollowerUserInteraction::saveUserPath(const hydroqguipath::GuiPath &guiPath, int numLoops, float timeOffset)
{   
    QString fileName = QFileDialog::getSaveFileName( 0, "Choose a filename to save under", userPathFileName_, "*.txt");
    
    if (!fileName.isEmpty())
    {
        userPathFileHandler_->savePath( fileName, guiPath, numLoops, timeOffset );
        userPathFileName_ = fileName;
        haveUserPathFileName_ = true;
    }
}

void 
PathFollowerUserInteraction::loadUserPath(hydroqguipath::GuiPath &guiPath)
{
    QString fileName = QFileDialog::getOpenFileName( 0, "Choose a path file to open", userPathFileName_, "*.txt");     
    
    if (!fileName.isEmpty())
        userPathFileHandler_->loadPath( fileName, guiPath );  
}

void 
PathFollowerUserInteraction::loadPreviousUserPath( hydroqguipath::GuiPath &guiPath )
{
    userPathFileHandler_->loadPreviousPath( guiPath );
}

}

