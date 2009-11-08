/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROQGUI_MOUSEEVENTMANAGER_H
#define HYDROQGUI_MOUSEEVENTMANAGER_H

#include <hydroutil/uncopyable.h>

namespace hydroqguielementutil {

class GuiElement;

//!
//! @brief Manages sharing of mouse events: a global resource.
//!
//! Individual GuiElements must register to be the One True receiver of mouse events.
//!
//! @author Alex Brooks
//!
class MouseEventManager : public hydroutil::Uncopyable
{

public: 

    MouseEventManager()
        : mouseEventReceiver_(NULL)
        {}

    bool mouseEventReceiverIsSet() const { return mouseEventReceiver_!=NULL; }

    GuiElement *mouseEventReceiver() const { return mouseEventReceiver_; }

    bool requestBecomeMouseEventReceiver( GuiElement *requester );

    void relinquishMouseEventReceiver( GuiElement *relinquisher );

private: 

    // NULL means no-one is receiving mouse events
    GuiElement *mouseEventReceiver_;

};

}

#endif
