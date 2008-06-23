/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <QtGui>

#include <orcaice/orcaice.h>
#include <orcaqt/orcaqt.h>

#include "mainwin.h"
#include <orcaqgui/guiicons.h>

using namespace std;

namespace orcaqgui {

MainWindow::MainWindow( 
    std::string                        title,   
    ScreenDumpParams                   screenDumpParams,
    const std::vector<std::string>    &supportedInterfaces )
    : screenDumpParams_(screenDumpParams),
      supportedInterfaces_(supportedInterfaces)
{
    setWindowTitle( title.c_str() );
    setWindowIcon ( QPixmap(orcaqt::orca2_2x3_yellow_130_xpm) );

    //
    // setup all the menus/toolbars etc.
    //
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

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QString aboutAppTitle = "About ";
    aboutAppTitle += title.c_str();
    helpMenu->addAction(aboutAppTitle, this, SLOT(aboutApp()), Qt::Key_F1 );
    helpMenu->addAction("About &Orca", this, SLOT(aboutOrca()), Qt::SHIFT | Qt::Key_F1 );

    // screen capture
    screenDumpCounter_ = 0;
    screenCaptureTimer_ = new QTimer( this );
    screenCaptureTimer_->setInterval( screenDumpParams_.captureTimerInterval );
    connect( screenCaptureTimer_,SIGNAL(timeout()), this,SLOT(grabWindow()) );
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
MainWindow::showDialogMsg( MessageType type, const QString &message )
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
MainWindow::showStatusMsg( MessageType type, const QString &msg )
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

}
