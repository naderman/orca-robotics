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
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguipath/wptolerancesdialog.h>
#include <orcaqgui2dfactory/pathplanner2delement.h>
#include <orcaqgui2dfactory/pathconversionutil.h>
#include "pathplanneruserinteraction.h"

using namespace std;

namespace orcaqgui2d {
    
PathPlannerUserInteraction::PathPlannerUserInteraction( PathPlanner2dElement                    *ppElement,
                                                        const std::string                       &proxyString,
                                                        hydroqguielementutil::IHumanManager     &humanManager,
                                                        hydroqguielementutil::MouseEventManager &mouseEventManager,
                                                        PathPainter                             &painter,
                                                        const orcaice::Context                  &context )
    : ppElement_(ppElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
      painter_(painter),
      context_(context),
      ifacePathFileName_("/tmp"),
      haveIfacePathFileName_(false),
      gotMode_(false)
{
    wpSettings_ = readWaypointSettings( context_.properties(), context_.tag() );
    buttons_.reset( new hydroqguipath::PathplannerButtons( this, humanManager, proxyString ) );
    ifacePathFileHandler_.reset( new PathFileHandler( humanManager ) );
}


void 
PathPlannerUserInteraction::setFocus( bool inFocus )
{
    if (inFocus) {
        if (buttons_.get()==0) {
            buttons_.reset( new hydroqguipath::PathplannerButtons( this, humanManager_, proxyString_ ) );
        }
    } else {
        buttons_.reset(0);
    }
}

void 
PathPlannerUserInteraction::paint( QPainter *p )
{
    if ( pathInput_.get() ) 
        pathInput_->paint(p);
}

void PathPlannerUserInteraction::waypointSettingsDialog()
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
PathPlannerUserInteraction::waypointModeSelected()
{
    if ( gotMode_ ) return;
    gotMode_ = mouseEventManager_.requestBecomeMouseEventReceiver( ppElement_ );

    if ( !gotMode_ )
    {
        humanManager_.showDialogWarning( "Couldn't take over the mode for PathPlanner waypoints!" );
        return;
    }

    pathInput_.reset( new PathPlannerInput( *this, &wpSettings_, humanManager_ ) );
    pathInput_->setUseTransparency( useTransparency_ );
    buttons_->setWpButton( true );   
}

void
PathPlannerUserInteraction::setUseTransparency( bool useTransparency )
{ 
    useTransparency_ = useTransparency;
    if ( pathInput_.get() ) 
        pathInput_->setUseTransparency( useTransparency ); 
}

void 
PathPlannerUserInteraction::send()
{
    if ( pathInput_.get() )
        ppElement_->sendPath( *pathInput_.get() );
    cancel();
}

void 
PathPlannerUserInteraction::cancel()
{
    if ( gotMode_ )
    {
        mouseEventManager_.relinquishMouseEventReceiver( ppElement_ );
        noLongerMouseEventReceiver();
    }
}
void
PathPlannerUserInteraction::noLongerMouseEventReceiver()
{
    assert( pathInput_.get() );
    pathInput_.reset(0);
    buttons_->setWpButton( false );
    gotMode_ = false;
}

void 
PathPlannerUserInteraction::savePathAs()
{
    QString fileName = QFileDialog::getSaveFileName(
            0,
            "Choose a filename to save under",
            ifacePathFileName_,
            "*.txt");
    
    if (!fileName.isEmpty())
    {
        ifacePathFileHandler_->savePath( fileName, painter_.currentPath() );
        haveIfacePathFileName_ = true;
    }
}

void 
PathPlannerUserInteraction::savePath()
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
