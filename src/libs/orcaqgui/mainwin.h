/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAQGUI_MAIN_WINDOW_H
#define ORCA_ORCAQGUI_MAIN_WINDOW_H

#include <vector>
#include <QMainWindow>

#include <hydroqguielementutil/ihumanmanager.h>
#include <orcaice/context.h>
#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaqguielementmodelview/guielementview.h>

class QTimer;

namespace orcaqgui {

//!
//! Provides the framework in which all other widgets sit.
//! It also handles screen capture.
//!
//! Call 'setCentralWidget()' to set the central widget.
//!
class MainWindow : public QMainWindow, 
                   public hydroqguielementutil::IHumanManager
{
    Q_OBJECT
public:

    //! configuration parameters for screen capture
    struct ScreenDumpParams {
        int topPad;
        int sidePad;
        int bottomPad;
        std::string dumpPath;
        int captureTimerInterval;
    };

    MainWindow( std::string                        title,   
                ScreenDumpParams                   screenDumpParams,
                const std::vector<std::string>    &supportedInterfaces );

    // Inherited from hydroqguielementutil::IHumanManager
    virtual void showDialogMsg( hydroqguielementutil::IHumanManager::MessageType type, const QString &msg );
    virtual void showStatusMsg( hydroqguielementutil::IHumanManager::MessageType type, const QString &msg );
    virtual QMenu      *fileMenu() { return fileMenu_; }
    virtual QMenu      *optionsMenu() { return optionsMenu_; }
    virtual QMenu      *displayMenu() { return displayMenu_; }
    virtual QMenuBar   *menuBar() { return QMainWindow::menuBar(); }
    virtual QToolBar   *toolBar() { return toolBar_; }
    virtual QStatusBar *statusBar() { return QMainWindow::statusBar(); }

private slots:

    // screen capture
    void toggleScreenCapture( bool capture );
    void grabWindow();

    void quit();
    void aboutOrca();
    void aboutApp();

private:

    // configuration parameters
    ScreenDumpParams screenDumpParams_;

    std::vector<std::string>    supportedInterfaces_;
    
    // screen capture
    void initScreenCapture();
    int screenDumpCounter_;
    QTimer* screenCaptureTimer_;
    
    QMenu* fileMenu_;
    QMenu* optionsMenu_;
    QMenu* displayMenu_;
    QToolBar *toolBar_;

    orcaice::Context        context_;
};

}

#endif
