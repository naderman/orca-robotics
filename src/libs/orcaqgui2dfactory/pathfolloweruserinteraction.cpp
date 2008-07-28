/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

PathFollowerUserInteraction::PathFollowerUserInteraction( PathFollower2dElement                    *pfElement,
                                                          const std::string                        &proxyString,
                                                          hydroqguielementutil::IHumanManager      &humanManager,
                                                          hydroqguielementutil::MouseEventManager  &mouseEventManager,
                                                          hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                                          const hydroqgui::GuiElementSet           &guiElementSet,
                                                          const PathPainter                        &painter,
                                                          const orcaice::Context                   &context,
                                                          const PathFollowerInputFactory           *inputFactory )
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
      numAutoPathDumps_(0),
      loadPreviousPathFilename_(""),
      gotMode_(false)
{
    wpSettings_ = readWaypointSettings( context.properties(), context.tag() );
    buttons_.reset( new PathfollowerButtons( this, humanManager, shortcutKeyManager, proxyString ) );
    ifacePathFileHandler_.reset( new PathFileHandler( humanManager ) );
}

void 
PathFollowerUserInteraction::setFocus( bool inFocus )
{
    if ( inFocus )
    {
        if ( !buttons_.get() ) 
        {
            buttons_.reset( new PathfollowerButtons( this, 
                                                     humanManager_, 
                                                     shortcutKeyManager_, 
                                                     proxyString_ ) );
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
PathFollowerUserInteraction::waypointModeSelected()
{
    if ( gotMode_ ) return;
    gotMode_ = mouseEventManager_.requestBecomeMouseEventReceiver( pfElement_ );
    
    if ( !gotMode_ )
    {
        humanManager_.showDialogWarning( "Couldn't take over the mode for PathFollower waypoints!" );
        return;
    }
    
    pathInput_ = inputFactory_->createPathFollowerInput( *this, 
                                                         &wpSettings_, 
                                                          humanManager_, 
                                                          loadPreviousPathFilename_ );
    
    pathInput_->setUseTransparency( useTransparency_ );
    buttons_->setWpButton( true );
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
    cout<<"TRACE(PathFollowerUserInteraction): send()" << endl;
    
    if ( !pathInput_.get() ) 
    {
        humanManager_.showDialogWarning( "Not in path input mode!" );
        return;
    }
        
    // save path to file automatically
    char buffer [5];
    sprintf(buffer,"%05d",numAutoPathDumps_++);
    QString filename = readDumpPath(context_.properties(), context_.tag()) + "/pathdump" + QString(buffer) + ".txt";
    pathInput_->savePath( filename );
    loadPreviousPathFilename_ = filename;
    
    pfElement_->sendPath( pathInput_.get(), readActivateImmediately(context_.properties(), context_.tag()) );
    
    cancel();
}

void 
PathFollowerUserInteraction::cancel()
{
    cout<<"TRACE(PathFollowerUserInteraction): cancel()" << endl;
    if ( gotMode_ )
    {
        mouseEventManager_.relinquishMouseEventReceiver( pfElement_ );
        noLongerMouseEventReceiver();
    }
}

void 
PathFollowerUserInteraction::go()
{
    cout<<"TRACE(PathFollowerUserInteraction): go()" << endl;
    pfElement_->go();
}

void 
PathFollowerUserInteraction::stop()
{
    cout<<"TRACE(PathFollowerUserInteraction): stop()" << endl;
    pfElement_->stop();
}

void 
PathFollowerUserInteraction::allGo()
{
    cout<<"TRACE(PathFollowerUserInteraction): allGo()" << endl;
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
PathFollowerUserInteraction::allStop()
{
    cout<<"TRACE(PathFollowerUserInteraction): allStop()" << endl;
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
PathFollowerUserInteraction::noLongerMouseEventReceiver()
{
    cout << "TRACE(PathFollowerUserInteraction): noLongerMouseEventReceiver()" << endl;
    pathInput_.reset(0);
    buttons_->setWpButton( false );
    gotMode_ = false;
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
    {   
        savePathAs();
    }
    else
    {
        ifacePathFileHandler_->savePath( ifacePathFileName_, painter_.currentPath() );
    }
}

}

