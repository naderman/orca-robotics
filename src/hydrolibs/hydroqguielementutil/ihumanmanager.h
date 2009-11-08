/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROQGUI_IHUMANMANAGER_H
#define HYDROQGUI_IHUMANMANAGER_H

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <hydroutil/uncopyable.h>

namespace hydroqguielementutil {

//!
//! @brief Interface for managing the humans: allows display of messages and access to menus/tool-bars
//!
//! @author Alex Brooks
//!
class IHumanManager : public hydroutil::Uncopyable
{

public: 

    enum MessageType
    {
        Information,
        Warning,
        Error    
    };

    virtual ~IHumanManager() {}

    //! Display the message in a dialog box
    virtual void showDialogMsg( MessageType type, const QString &msg ) = 0;
    //! Display the message in a status bar
    virtual void showStatusMsg( MessageType type, const QString &msg ) = 0;

    // convenience functions for those above
    void showDialogInformation( const QString &msg ) { showDialogMsg( Information, msg ); }
    void showDialogWarning( const QString &msg )     { showDialogMsg( Warning, msg ); }
    void showDialogError( const QString &msg )       { showDialogMsg( Error, msg ); }

    void showStatusInformation( const QString &msg ) { showStatusMsg( Information, msg ); }
    void showStatusWarning( const QString &msg )     { showStatusMsg( Warning, msg ); }
    void showStatusError( const QString &msg )       { showStatusMsg( Error, msg ); }

    //! Others can add things to the menus/toolbars.
    //! Play nicely with each other!
    virtual QMenu      *fileMenu()=0;
    virtual QMenu      *optionsMenu()=0;
    virtual QMenu      *displayMenu()=0;
    virtual QMenuBar   *menuBar()=0;
    virtual QToolBar   *toolBar()=0;
    virtual QStatusBar *statusBar()=0;

private: 

};

}

#endif
