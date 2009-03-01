/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "selectableelementwidget.h"
#include <iostream>
#include <cmath>        // for floor()
#include <QtGui>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaqt/orcaqt.h>
#include <orcaqcompmodel/orcaqcm.h>
#include <orcaqguielementmodelview/regselectview.h>

using namespace std;

namespace orcaqgui {

SelectableElementWidget::SelectableElementWidget( hydroqgui::PlatformFocusManager &platformFocusManager,
                                                  hydroiceutil::JobQueue             &jobQueue,
                                                  const orcaice::Context          &context,
                                                  orcaqgemv::GuiElementModel      *guiElementModel,
                                                  QMainWindow                     &mainWindow,
                                                  int                              regRefreshPeriodSec,
                                                  QWidget                         *parent )
    : QSplitter(parent),
      jobQueue_(jobQueue),
      context_(context)
{
    setOrientation( Qt::Vertical );


    // Model
    regModel_ = new orcaqcm::OcmModel();
    // Delegate
    regDelegate_ = new orcaqcm::OcmDelegate();
    // View
    regView_ = new orcaqgemv::RegSelectView(this);
    regView_->setModel( regModel_ );
    regView_->setItemDelegate(regDelegate_);
    regView_->header()->setMovable(true);
    regView_->setUniformRowHeights(true);
    regView_->setMinimumWidth( 300 );
    regView_->setSelectionMode ( QAbstractItemView::ExtendedSelection );
    regView_->setColumnWidth( 0, 200 );

    platformFocusCombo_ = new PlatformFocusCombo(platformFocusManager,this);

    // Bottom part: element view
    elemView_ = new orcaqgemv::GuiElementView(this);
    elemView_->setModel( guiElementModel );
    elemView_->horizontalHeader()->setMovable(true);
    elemView_->verticalHeader()->hide();
    elemView_->setAlternatingRowColors(true);
    
    // Give the model a pointer to the view
    guiElementModel->setView( elemView_ );
    
    QObject::connect( regView_,SIGNAL(newSelection( const QList<QStringList> & )),
                      guiElementModel,SLOT(createGuiElementFromSelection( const QList<QStringList> & )) );

    if ( regRefreshPeriodSec > 0 )
    {
        regTimer_ = new QTimer( this );
        QObject::connect( regTimer_,SIGNAL(timeout()), this,SLOT(updateRegistryView()) );
        regTimer_->start( 1000 * regRefreshPeriodSec );
    }

    // Connect the element model and the platformFocusCombo_
    QObject::connect( guiElementModel,
                      SIGNAL(newPlatform(const QString&)),
                      platformFocusCombo_,
                      SLOT(addPlatformToList(const QString&)) );    
    QObject::connect( guiElementModel,
                      SIGNAL(platformNeedsRemoval(const QString&)),
                      platformFocusCombo_,
                      SLOT(removePlatformFromList(const QString&)) );

    // refresh right now
    updateRegistryView();

    //
    // Set up registry menu
    //
    QMenu *registryMenu = mainWindow.menuBar()->addMenu("&Registry");
    registryMenu->addAction("Toggle &Disconnected", regView_, SLOT(toggleDisconnected()), QKeySequence("Ctrl+`") );
    registryMenu->addAction("Toggle &Standard", regView_, SLOT(toggleStandard()), QKeySequence("Ctrl+0") );
    QMenu *levelSubMenu = registryMenu->addMenu("Show &Level");
    levelSubMenu->addAction("&Registries", regView_, SLOT(showLevelOne()), QKeySequence("Ctrl+1") );
    levelSubMenu->addAction("&Platforms", regView_, SLOT(showLevelTwo()), QKeySequence("Ctrl+2") );
    levelSubMenu->addAction("&Components", regView_, SLOT(showLevelThree()), QKeySequence("Ctrl+3") );
    levelSubMenu->addAction("&Interfaces", regView_, SLOT(showLevelFour()), QKeySequence("Ctrl+4") );
    registryMenu->addSeparator();
    registryMenu->addAction("&Update", this, SLOT(updateRegistryView()), QKeySequence("F5") );
    registryMenu->addAction("&Reload", this, SLOT(reloadRegistryView()), QKeySequence("Ctrl+L") );    
}
                     
void
SelectableElementWidget::updateRegistryView()
{
    string locatorString = context_.communicator()->getDefaultLocator()->ice_toString();

    hydroiceutil::JobPtr job = new orcaqcm::GetComponentsJob( qApp, regModel_, context_, locatorString );
    jobQueue_.add( job );
}

void
SelectableElementWidget::reloadRegistryView()
{
    // first clear the model
    regModel_->clear();

    // now update the view
    updateRegistryView();
}

}
