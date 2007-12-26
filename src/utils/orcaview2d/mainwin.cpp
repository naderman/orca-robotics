/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>        // for floor()

#include <QtGui>

#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaqt/orcaqt.h>
#include <orcaqcm/orcaqcm.h>

#include "mainwin.h"
#include <orcaqguielementmodelview/regselectview.h>
#include <orcaqgui/guiicons.h>

using namespace std;

MainWindow::MainWindow( 
    std::string                        title,   
    ScreenDumpParams                   screenDumpParams,
    int                                displayRefreshTime,
    const std::vector<std::string>    &supportedInterfaces,
    hydroutil::JobQueue               *jobQueue,
    const orcaice::Context            &context)
    : screenDumpParams_(screenDumpParams),
      displayRefreshTime_(displayRefreshTime),
      elemModel_(NULL),
      displayView_(NULL),
      supportedInterfaces_(supportedInterfaces),
      firstTime_(true),
      jobQueue_(jobQueue),
      context_(context)
{
    setWindowTitle(title.c_str());
    setWindowIcon ( QPixmap(orcaqt::orca2_2x3_yellow_130_xpm) );
    
    //
    // Widget
    //
    win_ = new QSplitter();
    win_->setOrientation(Qt::Horizontal);
    win_->setMinimumHeight( 500 );
    setCentralWidget(win_);

    leftSide_ = new QSplitter(win_);
    leftSide_->setOrientation(Qt::Vertical);

    //
    // Select-from-Registry widget
    //
    // Model
    regModel_ = new orcaqcm::OcmModel();
    // Delegate
    regDelegate_ = new orcaqcm::OcmDelegate();
    // View
    regView_ = new orcaqgemv::RegSelectView(leftSide_);
    regView_->setModel( regModel_ );
    regView_->setItemDelegate(regDelegate_);
    //regView_->setSelectionModel(selections_);
    regView_->header()->setMovable(true);
    //regView_->setAlternatingRowColors(true);
    regView_->setUniformRowHeights(true);
    regView_->setMinimumWidth( 300 );
    regView_->setSelectionMode ( QAbstractItemView::ExtendedSelection );
    regView_->setColumnWidth( 0, 200 );
            
    platformCombo_ = new QComboBox(leftSide_);
    platformCombo_->setMinimumHeight(20);
    platformCombo_->setMaximumHeight(20);
    platformCombo_->setFont( QFont(platformCombo_->font().family(), 12, QFont::Bold) );
    connect(platformCombo_,SIGNAL(highlighted(const QString&)),this, SLOT(changePlatformFocus(const QString&)));

    // setup all the menus/toolbars etc.
    setupInterface();
}


QWidget *
MainWindow::displayViewParent()
{ 
    return win_; 
}

void MainWindow::init( orcaqgemv::GuiElementModel *guiElemModel,
                       QWidget                    *displayView )
{
    //
    // Active-GUI-Elements widget
    //
    // Model
    elemModel_ = guiElemModel;
    displayMenu_->addAction("&Remove All GUI Elements", elemModel_, SLOT(removeAllGuiElements()) );
    
    //
    // output display widget
    //
    displayView_ = displayView;
    
    displayView_->setShown( true );

    win_->adjustSize();
    
    QObject::connect( elemModel_, SIGNAL(newPlatform(const QString&)), this, SLOT(addPlatformToList(const QString&)) );    
    QObject::connect( elemModel_, SIGNAL(platformNeedsRemoval(const QString&)), this, SLOT(removePlatformFromList(const QString&)) );

    // Bottom left part: element view
    elemView_ = new orcaqgemv::GuiElementView(leftSide_);
    elemView_->setModel( elemModel_ );
    elemView_->horizontalHeader()->setMovable(true);
    elemView_->verticalHeader()->hide();
    elemView_->setAlternatingRowColors(true);
    
    // Give the model a pointer to the view
    elemModel_->setView( elemView_ );
    
    QObject::connect( regView_,SIGNAL(newSelection( const QList<QStringList> & )),
                      elemModel_,SLOT(createGuiElementFromSelection( const QList<QStringList> & )) );

    regTimer_ = new QTimer( this );
    QObject::connect( regTimer_,SIGNAL(timeout()), this,SLOT(refreshRegistryView()) );
    regTimer_->start( 10*1000 );

    statusBar()->showMessage( "Initialized", 2000 );

    // refresh right now
    updateRegistryView();

    displayTimer_ = new QTimer( this );
    QObject::connect( displayTimer_,SIGNAL(timeout()), this,SLOT(updateDisplayView()) );
    displayTimer_->start( displayRefreshTime_ );

    // screen capture
    screenDumpCounter_ = 0;
    screenCaptureTimer_ = new QTimer( this );
    screenCaptureTimer_->setInterval( screenDumpParams_.captureTimerInterval );
    connect( screenCaptureTimer_,SIGNAL(timeout()), this,SLOT(grabWindow()) );
}

void
MainWindow::setupInterface()
{
    fileMenu_ = menuBar()->addMenu("&File");
    optionsMenu_ = menuBar()->addMenu("&Options");
    displayMenu_ = menuBar()->addMenu("&Display");
    toolBar_ = addToolBar("Stuff");

    QPixmap exitIcon(exit_xpm);
    fileMenu_->addAction(exitIcon, "&Quit", this, SLOT(quit()), Qt::CTRL | Qt::Key_Q );

    QPixmap captureIcon( screendump_xpm );
    QAction* screenCapture = new QAction(captureIcon,tr("&Screen Capture"),this);
    screenCapture->setCheckable(true);
    connect(screenCapture,SIGNAL(toggled(bool)), this, SLOT(toggleScreenCapture(bool)) );
    screenCapture->setShortcut( QKeySequence("Ctrl+Alt+S") );
    displayMenu_->addAction(screenCapture);

    QMenu *registryMenu = menuBar()->addMenu("&Registry");
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

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("About Orca&View", this, SLOT(aboutApp()), Qt::Key_F1 );
    helpMenu->addAction("About &Orca", this, SLOT(aboutOrca()), Qt::SHIFT | Qt::Key_F1 );
    
#if 0    
    QPixmap selectIcon(select_xpm);
#endif    

}

void
MainWindow::updateRegistryView()
{
    string locatorString = context_.communicator()->getDefaultLocator()->ice_toString();

    hydroutil::JobPtr job = new orcaqcm::GetComponentsJob( qApp, regModel_, context_, locatorString );
    jobQueue_->add( job );
}

void
MainWindow::reloadRegistryView()
{
    // first clear the model
    regModel_->clear();

    // now update the view
    updateRegistryView();
}

void
MainWindow::updateDisplayView()
{
    elemModel_->updateGuiElements();

    // issue a QPaintEvent, all painting must be done from paintEvent().
    displayView_->update();
}
    
void
MainWindow::quit()
{
    if( QMessageBox::information(this, "Quit...",
                                      "Do your really want to quit?",
                                      QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok )
    {
        close();
    }
}

void
MainWindow::showBoxMsg( MessageType type, QString message )
{
    if (type==Information)
    {
        QMessageBox::information(this, "INFO", message, QMessageBox::Ok);
        cout<<"TRACE(mainwin.cpp): INFO: " << message.toStdString() << endl;
    }
    else if (type==Warning)
    {
        QMessageBox::warning(this, "WARNING", message, QMessageBox::Ok, QMessageBox::NoButton);
        cout<<"TRACE(mainwin.cpp): WARNING: " << message.toStdString() << endl;
    }
    else if (type==Error)
    {
        QMessageBox::critical(this, "ERROR", message, QMessageBox::Ok, QMessageBox::NoButton);
        cout<<"TRACE(mainwin.cpp): ERROR: " << message.toStdString() << endl;
    }
}

void 
MainWindow::showStatusMsg( MessageType type, QString msg )
{
    statusBar()->setFont( QFont(statusBar()->font().family(), 12, QFont::Bold) );
    
    QColor color;
    QString str;
    if (type==Information)
    {
        color = Qt::black; str = "INFO: ";
    }
    else if (type==Warning)
    {
        color = Qt::darkYellow; str = "WARNING: ";
    }
    else if (type==Error)
    {
        color = Qt::darkRed; str = "ERROR: ";
    }
    
    QPalette plt;
    plt.setColor(QPalette::WindowText, color);
    statusBar()->setPalette( plt );
    
    statusBar()->showMessage( str+msg, 2000 );
    cout<<"TRACE(mainwin.cpp): " << (str+msg).toStdString() << endl;
}

void
MainWindow::aboutOrca()
{
    orcaqt::aboutOrca( this );
}

void
MainWindow::aboutApp()
{
    QString text;
    text += "Connects to Orca components and displays\n";
    text += "information arriving from their interfaces.\n";
    text += "Supported interfaces:\n";
    for ( unsigned int i=0; i<supportedInterfaces_.size(); ++i ) {
        text += "    ";
        text += QString::fromStdString( supportedInterfaces_[i] );
        text += "\n";
    }
    orcaqt::aboutApp( this, "About OrcaView", text );
}

void 
MainWindow::addPlatformToList(const QString& platform)
{
    assert(platform!="");

    cout<<"TRACE(mainwin.cpp): received newPlatform signal: " << platform.toStdString() << endl;
    
    // if it gets here, the platform is a new one
    platformCombo_->addItem( platform );
}

void
MainWindow::removePlatformFromList(const QString& platform)
{
    cout<<"TRACE(mainwin.cpp): received removePlatformFromList signal: " << platform.toStdString() << endl;
    int index = platformCombo_->findText( platform );
    platformCombo_->removeItem( index );    
}


void 
MainWindow::changePlatformFocus(const QString& platform)
{
    elemModel_->changePlatformFocus( platform );
}

// AlexB: TODO: Commented this bit out coz I don't understand what's going on here.
//              Have to ask Tobi.
#if 0
void 
MainWindow::changePlatformFocusFromView(const QString& platform)
{
    cout << "TRACE(mainwin.cpp): changePlatformFocusFromView, platform: " << platform.toStdString() << endl;
    changePlatformFocus(platform);
    
    // set combobox as well
    int i = platformCombo_->findText( platform );
    platformCombo_->setCurrentIndex( i );
}
#endif

void 
MainWindow::toggleScreenCapture( bool capture )
{
    QString str;

    if (capture) 
    {
        showStatusMsg(Information, "Screen capture is on");
        screenCaptureTimer_->start();
    } 
    else 
    {
        showStatusMsg(Information, "Screen capture is off");
        screenCaptureTimer_->stop();
    }
    
}

void 
MainWindow::grabWindow()
{    
    // don't eat resources when iconified
    if ( !isVisible() ) return;

    char buffer [5];
    sprintf(buffer,"%05d",screenDumpCounter_++);
    QString filepath = QString(screenDumpParams_.dumpPath.c_str()) + "/orcaviewdump" + QString(buffer) + ".png";
    
    // Note: When tested, grabWindow did not return the window decorations.  This extra padding
    //       is to get it to do that.
    //       Some experimentation is required to work out good values (probably specific to window manager)
    //

    QPixmap pix = QPixmap::grabWindow( this->winId(),
                                       -screenDumpParams_.sidePad,
                                       -screenDumpParams_.topPad, 
                                       width() + 2*screenDumpParams_.sidePad, 
                                       height() + screenDumpParams_.topPad + screenDumpParams_.bottomPad );

    if ( !pix.save( filepath, "PNG", -1 ) )
    {   
        showStatusMsg(Error, "Failed to save screendump to " + filepath );
    }
    else
    {
        showStatusMsg(Information, "Saved screendump to " + filepath );    
    }
}
