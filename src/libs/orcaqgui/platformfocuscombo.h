#ifndef ORCAQGUI_PLATFORMFOCUSCOMBO_H
#define ORCAQGUI_PLATFORMFOCUSCOMBO_H

#include <hydroqgui/hydroqgui.h>
#include <QComboBox>

namespace orcaqgui {

//!
//! @brief A QCombo box allowing users to select the platform in focus.
//! 
//! @see hydroqgui::PlatformFocusManager
//!
//! @author Alex Brooks
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
