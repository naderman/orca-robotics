/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef BROWSER_EVENTS_H
#define BROWSER_EVENTS_H

#include <hydroiceutil/event.h>

namespace probe
{

enum EventType  
{
    Activate,
    Reload,
    Up,
    Top,
    Fault,
    Deactivate,
    Filter,
    Pick
};

class PickEvent : public hydroiceutil::Event
{
public:
    PickEvent( int i ) :
        Event( Pick ),
        pick(i) {};
    int pick;
};
typedef IceUtil::Handle<PickEvent> PickEventPtr;


} // namespace

#endif
