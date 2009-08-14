#ifndef HYDROQGUI_SHORTCUTKEYMANAGER_H
#define HYDROQGUI_SHORTCUTKEYMANAGER_H

#include <QWidget>
#include <QAction>
#include <QKeySequence>
#include <hydroutil/uncopyable.h>

namespace hydroqguielementutil {

namespace detail {
    class ShortcutKey;
}

//!
//! @brief manages sharing of shortcut keys (a global resource) between GuiElements.
//!
//! GuiElements can subscribe for shortcuts.  They must unsubscribe before they fall out of
//! scope, or a seg fault will occur.
//!
//! Shortcut Keys can either be owned:
//!  - exclusively (last-to-register gets triggered), or
//!  - inclusively (everyone gets triggered)
//!
//! @author Alex Brooks, Tobias Kaupp
//!
class ShortcutKeyManager : public hydroutil::Uncopyable
{

public: 

    ShortcutKeyManager( QWidget *mainWidget );

    ~ShortcutKeyManager();

    //! WARNING: if a QAction should have a shortcut (e.g. ESC), then do *not* assign 
    //! them in the guielement by doing toolBar()->setShortcut(QKeySequence(Qt::Esc)).
    //! Otherwise you'll end up with an ambiguity Qt can't resolve.
    //! Use this function instead.
    void subscribeToShortcutKey( QAction      *elementAction,
                                 QKeySequence  key,
                                 bool          ownExclusively,
                                 QObject      *triggeree  );

    //! WARNING: remember to unsubscribeFromShortcutKey in the destructor, otherwise segfaults will occur
    void unsubscribeFromShortcutKey( QKeySequence  key,
                                     QObject      *triggeree );

private: 

    // list of shorcut actions
    QList<detail::ShortcutKey*> shortcutKeys_;

    QWidget       *mainWidget_;
};

}

#endif
