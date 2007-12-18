/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef BROWSER_EVENTS_H
#define BROWSER_EVENTS_H

#include <hydroutil/eventqueue.h>

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

class ActivateEvent : public hydroutil::Event
{
public:
    ActivateEvent() : Event( Activate ) {};
};

class ReloadEvent : public hydroutil::Event
{
public:
    ReloadEvent() : Event( Reload ) {};
};

class UpEvent : public hydroutil::Event
{
public:
    UpEvent() : Event( Up ) {};
};

class TopEvent : public hydroutil::Event
{
public:
    TopEvent() : Event( Top ) {};
};

class FaultEvent : public hydroutil::Event
{
public:
    FaultEvent() : Event( Fault ) {};
};

class FilterEvent : public hydroutil::Event
{
public:
    FilterEvent() : Event( Filter ) {};
};

class DeactivateEvent : public hydroutil::Event
{
public:
    DeactivateEvent() : Event( Deactivate ) {};
};

class PickEvent : public hydroutil::Event
{
public:
    PickEvent( int pick ) :
        Event( Pick ),
        pick_(pick) {};

    int pick_;
};
typedef IceUtil::Handle<PickEvent> PickEventPtr;


} // namespace

#endif
