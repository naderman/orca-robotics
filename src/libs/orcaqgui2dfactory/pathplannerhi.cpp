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
#include <hydroqgui/guielementset.h>
#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaqgui2dfactory/pathplanner2delement.h>
#include <orcaqgui2dfactory/wptolerancesdialog.h>
#include "pathplannerhi.h"

using namespace std;

namespace orcaqgui2d {
    
PathPlannerHI::PathPlannerHI( PathPlanner2dElement         *ppElement,
                              string                        proxyString,
                              hydroqguielementutil::IHumanManager     &humanManager, 
                              hydroqguielementutil::MouseEventManager &mouseEventManager, 
                              PathPainter                  &painter,
                              WaypointSettings              wpSettings )
    : ppElement_(ppElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      mouseEventManager_(mouseEventManager),
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
    gotMode_ = mouseEventManager_.requestBecomeMouseEventReceiver( ppElement_ );

    if ( !gotMode_ )
    {
        humanManager_.showDialogMsg( hydroqguielementutil::IHumanManager::Warning, "Couldn't take over the mode for PathPlanner waypoints!" );
        return;
    }

    pathInput_ = new PathPlannerInput( *this, &wpSettings_, humanManager_ );
    pathInput_->setUseTransparency( useTransparency_ );
    buttons_->setWpButton( true );   
}

void
PathPlannerHI::setUseTransparency( bool useTransparency )
{ 
    useTransparency_ = useTransparency;
    if (pathInput_) 
        pathInput_->setUseTransparency( useTransparency ); 
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
        mouseEventManager_.relinquishMouseEventReceiver( ppElement_ );
        noLongerMouseEventReceiver();
    }
}
void
PathPlannerHI::noLongerMouseEventReceiver()
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
        painter_.savePath( fileName, &humanManager_ );
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
        painter_.savePath( pathFileName_, &humanManager_ );
    }
}
    
}
