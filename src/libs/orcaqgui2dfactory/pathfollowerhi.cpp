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
#include <hydroqgui/guielementset.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaqgui2dfactory/pathfollower2delement.h>
#include <orcaqgui2dfactory/wptolerancesdialog.h>

#include "pathfollowerhi.h"

using namespace std;

namespace orcaqgui2d {

PathFollowerHI::PathFollowerHI( PathFollower2dElement                    *pfElement,
                                const std::string                        &proxyString,
                                hydroqguielementutil::IHumanManager      &humanManager,
                                hydroqguielementutil::MouseEventManager  &mouseEventManager,
                                hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                                const hydroqgui::GuiElementSet           &guiElementSet,
                                const PathPainter                        &painter,
                                const orcaice::Context                   &context )
    : pfElement_(pfElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      shortcutKeyManager_(shortcutKeyManager),
      guiElementSet_(guiElementSet),
      painter_(painter),
      context_(context),
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
PathFollowerHI::setFocus( bool inFocus )
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
        humanManager_.showDialogWarning( "Couldn't take over the mode for PathFollower waypoints!" );
        return;
    }

    pathInput_.reset( new PathFollowerInput( *this, 
                                             &wpSettings_, 
                                              humanManager_, 
                                              loadPreviousPathFilename_ ) );
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
        humanManager_.showDialogWarning( "Not in path input mode!" );
        return;
    }
        
    // save path to file automatically
    char buffer [5];
    sprintf(buffer,"%05d",numAutoPathDumps_++);
    QString filename = readDumpPath(context_.properties(), context_.tag()) + "/pathdump" + QString(buffer) + ".txt";
    pathInput_->savePath( filename );
    loadPreviousPathFilename_ = filename;
    
    pfElement_->sendPath( *pathInput_, readActivateImmediately(context_.properties(), context_.tag()) );
    
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
    cout << "TRACE(PathFollowerHI): noLongerMouseEventReceiver()" << endl;
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
PathFollowerHI::savePath()
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
