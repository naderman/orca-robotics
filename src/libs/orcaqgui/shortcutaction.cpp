/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include "shortcutaction.h"
#include <iostream>

using namespace orcaqgui;
using namespace std;

ShortcutAction::ShortcutAction( QObject *parent, QKeySequence keySequence)
    : QAction( parent ),
      keySequence_(keySequence)
{
    setShortcut(keySequence);
    connect(this,SIGNAL(triggered()),this,SLOT(triggerElementActions()));
}

void 
ShortcutAction::subscribe( QAction *action )
{
    cout << "TRACE(shortcutaction.cpp): pushing back new element action" << endl;
    actionList_.push_back( action );
}

void
ShortcutAction::triggerElementActions()
{
    cout << "TRACE(shortcutaction.cpp): triggerElementActions: list has size: " << actionList_.size() << endl;
    for (int i=0; i<actionList_.size(); i++)
    {
        actionList_[i]->trigger();        
    }
}

