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

#include "shortcutaction.h"

using namespace orcaqgui;
using namespace std;

ShortcutAction::ShortcutAction( QKeySequence keySequence, bool isMultiple )
    :  QAction(this),
       keySequence_(keySequence),
       isMultiple_(isMultiple)
{
    setShortcut(keySequence);
    connect(this,SIGNAL(triggered()),this,SLOT(triggerElementActions()));
}

void 
ShortcutAction::subscribe( QObject *parent, QAction *action )
{
    cout << "TRACE(shortcutaction.cpp): inserting new element action" << endl;
    actionMap_.insert(parent,action);
}

void
ShortcutAction::unsubscribe( QObject *parent )
{
    int numRemoved = actionMap_.remove ( parent );
    cout << "TRACE(shortcutaction.cpp): Just removed " << numRemoved << " items from the map" << endl;
}

void
ShortcutAction::triggerElementActions()
{
    cout << "TRACE(shortcutaction.cpp): triggerElementActions: map has size: " << actionMap_.size() << endl;    
    QMultiMap<QObject*, QAction*>::const_iterator i = actionMap_.constBegin();
    
    // if we are not handling multiple shortcuts, just trigger the first one and return
    if (!isMultiple_) {
        i.value()->trigger();
        return;
    }
    
    while (i != actionMap_.constEnd()) 
    {
        i.value()->trigger();
        ++i;
    }
}

