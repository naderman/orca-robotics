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
#include <hydroqgui/exceptions.h>
#include "mouseeventmanager.h"

using namespace std;

namespace hydroqguielementutil {

bool
MouseEventManager::requestBecomeMouseEventReceiver( GuiElement *requester )
{
    if ( mouseEventReceiverIsSet() )
        mouseEventReceiver_->noLongerMouseEventReceiver();
    mouseEventReceiver_ = requester;
    return true;
}

void
MouseEventManager::relinquishMouseEventReceiver( GuiElement *relinquisher )
{
    if ( relinquisher==mouseEventReceiver_ ) 
        mouseEventReceiver_=NULL;
    else
        throw hydroqgui::Exception( ERROR_INFO, "Attempt to relinquish mode from non-owner!" );
}

}
