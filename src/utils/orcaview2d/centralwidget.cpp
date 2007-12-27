#include "centralwidget.h"
#include <iostream>
#include <cmath>        // for floor()
#include <QtGui>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaqt/orcaqt.h>
#include <orcaqcm/orcaqcm.h>
#include "mainwin.h"
#include <orcaqguielementmodelview/regselectview.h>

using namespace std;

CentralWidget::CentralWidget( hydroqgui::PlatformFocusManager &platformFocusManager,
                              int                              displayRefreshTime,
                              hydroutil::JobQueue             &jobQueue,
                              const orcaice::Context          &context )
    : displayRefreshTime_(displayRefreshTime)
{
    setOrientation(Qt::Horizontal);
    setMinimumHeight( 400 );

    leftSide_ = new SelectableElementWidget( platformFocusManager,
                                             jobQueue,
                                             context,
                                             this );
}

void 
CentralWidget::init( orcaqgemv::GuiElementModel *guiElemModel,
                     QWidget                    *displayView,
                     QMainWindow                &mainWindow )
{
    leftSide_->init( guiElemModel, mainWindow );

    elemModel_ = guiElemModel;
    
    //
    // output display widget
    //
    displayView_ = displayView;
    displayView_->setShown( true );
    adjustSize();
    
    displayTimer_ = new QTimer( this );
    QObject::connect( displayTimer_,SIGNAL(timeout()), this,SLOT(updateDisplayView()) );
    displayTimer_->start( displayRefreshTime_ );
}

void
CentralWidget::updateDisplayView()
{
    elemModel_->updateGuiElements();

    // issue a QPaintEvent, all painting must be done from paintEvent().
    displayView_->update();
}
    
