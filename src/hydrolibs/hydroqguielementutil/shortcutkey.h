/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef HYDROQGUI_SHORTCUTKEY_H
#define HYDROQGUI_SHORTCUTKEY_H

#include <QAction>
#include <QKeySequence>
#include <QList>

namespace hydroqguielementutil {

namespace detail{ 

//! This class manages actions for a single shortcut key. Standard QAction
//! semantics are followed in terms enabled/disabled: shortcuts do not work
//! for currently disabled actions.
//!
//!  e.g. ESC. Shortcuts need to be unique, that's
//! why we need this proxy. Actions can be subscribed to ShortcutKey and
//! are stored in a multimap with their parent as a key. When the parent dies,
//! we need to unsubscribe (Qt style: in the destructor), so we can delete
//! the pointer from the map.
//!
//! author: Tobias Kaupp, Alex Brooks
class ShortcutKey : public QAction
{
    Q_OBJECT
public:
    
    //! If ownExclusively, only the most-recently-subscribed QObject is triggered.
    //! Otherwise, a key-press will trigger _all_ QObjects subscribed for that key.
    ShortcutKey( QKeySequence keySequence, bool ownExclusively, QObject *parent );
    ~ShortcutKey() {};
    void subscribe( QObject *parent, QAction *action, bool ownExclusively );
    void unsubscribe( QObject *parent );
    QKeySequence key() { return keySequence_; };
    
    int numSubscribers() const { return actionMap_.size(); }

public slots:
    void triggerElementActions();
    
private:
    QKeySequence keySequence_;
    bool isOwnedExclusively_;

    // This actionMap_ is a QMultiMap: it allows multiple QAction's
    // to be associated with a single QObject.
    QMultiMap<QObject*,QAction*> actionMap_;
};

}
    
}
#endif
