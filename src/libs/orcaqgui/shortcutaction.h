/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAQGUI_SHORTCUTACTION_H
#define ORCAQGUI_SHORTCUTACTION_H

#include <QAction>
#include <QKeySequence>
#include <QList>

namespace orcaqgui {

class ShortcutAction : public QAction
{
    Q_OBJECT
    public:
        ShortcutAction(QObject * parent, QKeySequence keySequence);
        ~ShortcutAction() {};
        void subscribe( QAction *action );
        QKeySequence key() { return keySequence_; };
        
    public slots:
        void triggerElementActions();
        
    private:
        QKeySequence keySequence_;
        QList<QAction*> actionList_;
};

}
#endif
