/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#ifndef ORCAQGUI_PLATFORMFOCUSCOMBO_H
#define ORCAQGUI_PLATFORMFOCUSCOMBO_H

#include <hydroqgui/platformfocusmanager.h>
#include <QComboBox>

namespace orcaqgui {

//!
//! @brief A QCombo box allowing users to select the platform in focus.
//! 
//! @see hydroqgui::PlatformFocusManager
//!
//! @author Tobias Kaupp, Alex Brooks
//!
class PlatformFocusCombo : public QComboBox,
                           public hydroqgui::PlatformFocusChangeReceiver
{
    Q_OBJECT
public: 

    PlatformFocusCombo( hydroqgui::PlatformFocusManager &platformFocusManager,
                        QWidget *parent=NULL );
    void platformFocusChanged( const QString &newPlatformName );

public slots:

    void changePlatformFocus(const QString&);
    void addPlatformToList(const QString&);
    void removePlatformFromList(const QString&);

private: 

    hydroqgui::PlatformFocusManager &platformFocusManager_;
};

}

#endif
