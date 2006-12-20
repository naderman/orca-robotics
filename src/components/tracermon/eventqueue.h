/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_EVENT_QUEUE_H
#define ORCA2_TRACERMON_EVENT_QUEUE_H

#include <list>

#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>

namespace tracermon
{

class Event : public IceUtil::Shared
{
public:
    Event( int type ) : type_(type) {};

    int type() const { return type_; };
private:
    int type_;
};

typedef IceUtil::Handle<Event> EventPtr;


class EventQueue : public IceUtil::Shared, public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    void add( const EventPtr & e );
    bool timedGet( EventPtr & e, int timeoutMs=-1 );

private:
    std::list<EventPtr> events_;

    void get( EventPtr & e );
};

typedef IceUtil::Handle<EventQueue> EventQueuePtr;

} // namespace

#endif
