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
#include <map>
#include <cmath>        // for floor()

#include <QtGui>

#include <orcaqcm/ocmdelegate.h>
#include <orcaqt/orcaqt.h>

#include "mainwin.h"
#include "regtreeview.h"
#include "propertywidget.h"


using namespace std;

MainWindow::MainWindow( orcaqcm::NetworkHandler *networkHandler, double refreshInterval,
                        QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
      networkHandler_(networkHandler)
{
    setWindowTitle("Orca2: Registry View");
    setWindowIcon ( QPixmap(orcaqt::orca2_2x3_yellow_130_xpm) );
    //
    // Widget
    //
    QSplitter* split = new QSplitter();
    split->setOrientation(Qt::Vertical);
    split->setMinimumWidth( 400 );
    setCentralWidget(split);

    //
    // Model
    //
    //model_ = new orcaqcm::OcmTreeModel();
    modelHandler_ = new orcaqcm::ModelHandler( *networkHandler );
    
    //selections_ = new QItemSelectionModel(model_);

    //
    // Delegate
    //
    delegate_ = new orcaqcm::OcmDelegate();
    
    //
    // View
    //
    view_ = new RegTreeView(split);
    view_->setModel( modelHandler_->model() );
    view_->setItemDelegate(delegate_);
    //view_->setSelectionModel(selections_);
    view_->header()->setMovable(true);
    //view_->setAlternatingRowColors(true);
    view_->setUniformRowHeights(true);
    // enable sorting
    //connect( view_->header(),SIGNAL(sectionClicked(int)), model_,SLOT(sort(int)) );
    
    connect( view_,SIGNAL(propertiesRequested(const QString &)), this,SLOT(getProperties(const QString &)) );

    setupMenuBar();
    
    refreshTimer_ = new QTimer( this );
    connect( refreshTimer_,SIGNAL(timeout()), this,SLOT(updateRegistryView()) );
    //refreshTimer_->start( (int)floor(config.refreshInterval*1000.0) );
    refreshTimer_->start( (int)floor(refreshInterval*1000.0) );

    statusBar()->showMessage( "Initialized", 2000 );

    // refresh right now
    updateRegistryView();
}

void
MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Quit", this, SLOT(quit()), Qt::CTRL | Qt::Key_Q );

    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Toggle &Disconnected", view_, SLOT(toggleDisconnected()), QKeySequence("Ctrl+`") );
    viewMenu->addAction("Toggle &Standard", view_, SLOT(toggleStandard()), QKeySequence("Ctrl+0") );
    QMenu *levelSubMenu = viewMenu->addMenu("Show &Level");
    levelSubMenu->addAction("&Registries", view_, SLOT(showLevelOne()), QKeySequence("Ctrl+1") );
    levelSubMenu->addAction("&Platforms", view_, SLOT(showLevelTwo()), QKeySequence("Ctrl+2") );
    levelSubMenu->addAction("&Components", view_, SLOT(showLevelThree()), QKeySequence("Ctrl+3") );
    levelSubMenu->addAction("&Interfaces", view_, SLOT(showLevelFour()), QKeySequence("Ctrl+4") );
    viewMenu->addSeparator();
    viewMenu->addAction("&Update", this, SLOT(updateRegistryView()), QKeySequence("F5") );
    viewMenu->addAction("&Reload", this, SLOT(reloadRegistryView()), QKeySequence("Ctrl+L") );
    
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("About &RegistryView", this, SLOT(aboutApp()));
    helpMenu->addAction("About &Orca", this, SLOT(aboutOrca()));

}

void
MainWindow::updateRegistryView()
{
    networkHandler_->getComponentInfo();
    
//     statusBar()->showMessage( "Downloaded a list of "+QString::number(compNumber)+" components.", 3000 );
}

void
MainWindow::reloadRegistryView()
{
    // first clear the model
    modelHandler_->clearModel();

    // now update the view
    updateRegistryView();
}

void
MainWindow::getProperties( const QString & proxy )
{
//     std::map<std::string,std::string> props = networkHandler_->getProperties( proxy.toStdString() );
// 
//     // debug
//     cout<<"MainWindow: got "<<props.size()<<" properties"<<endl;
//     for ( map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
//         cout<<it->first<<"\t\t"<<it->second<<endl;
//     }
// 
//     // convert to QMap
//     QMap<QString,QString> qprops;
//     for ( map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
//         qprops.insert( QString::fromStdString(it->first), QString::fromStdString(it->second) );
//     }
// 
//     PropertyWidget* propWidget = new PropertyWidget( qprops );
//     propWidget->show();
}

void
MainWindow::quit()
{
    close();
}

void
MainWindow::aboutOrca()
{
    orcaqt::aboutOrca( this );
}

void
MainWindow::aboutApp()
{
    QPixmap orcaIcon( orcaqt::orca2_2x3_yellow_130_xpm );

    QMessageBox mb( this );
    mb.setWindowTitle("About RegistryView");
    mb.setWindowIcon ( orcaIcon );
    mb.setText("Gives a view of the currently deployed\n"
               "Orca system based on the information from\n"
               "an IceGrid Registry.\n\n"
                "http://orca-robotics.sf.net\n" );
    mb.setIconPixmap( orcaIcon );
    mb.exec();
}
