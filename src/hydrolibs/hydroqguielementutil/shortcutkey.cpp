/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <sstream>
#include <assert.h>
#include <hydroqgui/exceptions.h>
#include "shortcutkey.h"

using namespace std;

namespace hydroqguielementutil {
namespace detail {

ShortcutKey::ShortcutKey( QKeySequence keySequence, bool ownExclusively, QObject *parent )
    :  QAction(parent),
       keySequence_(keySequence),
       isOwnedExclusively_(ownExclusively)
{
    setShortcut(keySequence);
    connect(this,SIGNAL(triggered()),this,SLOT(triggerElementActions()));
}

void 
ShortcutKey::subscribe( QObject *parent, QAction *action, bool ownExclusively )
{
//     cout << "TRACE(shortcutkey.cpp): " << keySequence_.toString().toStdString() 
//          << ": inserting new element action." << endl;

    if ( ownExclusively != isOwnedExclusively_ )
    {
        stringstream ss;
        ss << "For shortcut key: " << keySequence_.toString().toStdString()
           << ": mismatch in exclusivity.";
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }
    actionMap_.insert(parent,action);
}

void
ShortcutKey::unsubscribe( QObject *parent )
{
    int numRemoved = actionMap_.remove ( parent );
//     cout << "TRACE(shortcutkey.cpp): " << keySequence_.toString().toStdString() 
//          << ": Just removed " << numRemoved << " items from the map." << endl
//          << "  there are " << actionMap_.size() << " remaining." << endl;
    if ( numRemoved != 1 )
    {
        assert( !"Huh?  Shouldn't numRemoved always be 1?" );
    }
}

void
ShortcutKey::triggerElementActions()
{
//     cout << "TRACE(shortcutkey.cpp): triggerElementActions("<< keySequence_.toString().toStdString() 
//          << "): map has size: " << actionMap_.size() << endl;    
    QMultiMap<QObject*, QAction*>::const_iterator i = actionMap_.constBegin();
    
    // if we are not handling multiple shortcuts, just trigger the first one and return
    if ( isOwnedExclusively_ ) {
        if ( i.value()->isEnabled() )
            i.value()->trigger();
        return;
    }
    
    while (i != actionMap_.constEnd()) 
    {
        if ( i.value()->isEnabled() )
            i.value()->trigger();
        ++i;
    }
}

}
}
