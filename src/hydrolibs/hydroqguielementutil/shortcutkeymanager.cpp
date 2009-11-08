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
#include <hydroqguielementutil/shortcutkey.h>
#include "shortcutkeymanager.h"
#include <assert.h>

using namespace std;

namespace hydroqguielementutil {

ShortcutKeyManager::ShortcutKeyManager( QWidget *mainWidget )
    : mainWidget_(mainWidget)
{
}

ShortcutKeyManager::~ShortcutKeyManager()
{
    for ( int i=0; i < shortcutKeys_.size(); i++ )
    {
        delete shortcutKeys_[i];
    }
}

void
ShortcutKeyManager::subscribeToShortcutKey( QAction      *elementAction,
                                            QKeySequence  key,
                                            bool          ownExclusively,
                                            QObject      *triggeree )
{
//     cout << "TRACE(shortcutkeymanager.cpp): subscribeToShortcutKey("
//          << key.toString().toStdString()
//          << "): number of shortcutKeys " << shortcutKeys_.size() << endl;
    
    // check whether this shortcut already exists
    for (int i=0; i<shortcutKeys_.size(); i++)
    {
        if (shortcutKeys_[i]->key() == key)
        {
            // Subscribe along with existing subscribers
            shortcutKeys_[i]->subscribe( triggeree, elementAction, ownExclusively );
            return;
        }
    }
            
    // if we get here then the shortcut doesn't exist yet
    // cout << "TRACE(shortcutkeymanager.cpp): we have a new shortcut" << endl;
    detail::ShortcutKey *shortcutKey = new detail::ShortcutKey( key, ownExclusively, mainWidget_ );
    shortcutKey->subscribe( triggeree, elementAction, ownExclusively );
    // add the action to this widget, so it can catch key-presses
    mainWidget_->addAction(shortcutKey);
    
    // put it into the list, so it's persistent and can be checked for its key later
    shortcutKeys_.push_back( shortcutKey );
}

void 
ShortcutKeyManager::unsubscribeFromShortcutKey( QKeySequence key, QObject *triggeree )
{
    // cout << "ShortcutKeyManager: unsubscribeFromShortcutKey" << endl;
    
    for (int i=0; i<shortcutKeys_.size(); i++)
    {
        if (shortcutKeys_[i]->key() == key)
        {
            detail::ShortcutKey *shortcutKey = shortcutKeys_[i];
            shortcutKey->unsubscribe( triggeree );
            if ( shortcutKey->numSubscribers() == 0 )
            {
                shortcutKeys_.removeAt( i );
                delete shortcutKey;
            }
            return;
        }
    }

    // If we get to here then we tried to unsubscribe from a shortcut key that didn't exist.
    cout << "ShortcutKeyManager::"<<__func__<<": tried to unsubscribe from shortcut key that doesn't exist: " << key.toString().toStdString();
    assert( false && "There should always bea  key to unsubscribe from." );
}


}

