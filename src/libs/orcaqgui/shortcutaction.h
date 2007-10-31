/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAQGUI_SHORTCUTACTION_H
#define ORCAQGUI_SHORTCUTACTION_H

#include <QAction>
#include <QKeySequence>
#include <QList>

namespace orcaqgui {

// This class is an action proxy representing a bunch of actions with
// the same key shortcut, e.g. ESC. Shortcuts need to be unique, that's
// why we need this proxy. Actions can be subscribed to ShortcutAction and
// are stored in a multimap with their parent as a key. When the parent dies,
// we need to unsubscribe (Qt style: in the destructor), so we can delete
// the pointer from the map.
//
// author: Tobias Kaupp
class ShortcutAction : public QAction
{
    Q_OBJECT
    public:
        ShortcutAction( QKeySequence keySequence, bool isMultiple );
        ~ShortcutAction() {};
        void subscribe( QObject *parent, QAction *action );
        void unsubscribe( QObject *parent );
        QKeySequence key() { return keySequence_; };
        
    public slots:
        void triggerElementActions();
        
    private:
        QKeySequence keySequence_;
        bool isMultiple_;
        QMultiMap<QObject*,QAction*> actionMap_;
};

}
#endif
