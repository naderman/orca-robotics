/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

namespace orcaice
{

//! Generic event which can be added to the EventQueue.
class Event : public IceUtil::Shared
{
public:
    //! Constructor requires event type.
    Event( int type ) : type_(type) {};

    //! Returns event type.
    int type() const { return type_; };
private:
    int type_;
};

typedef IceUtil::Handle<Event> EventPtr;


/*!
    Thread-safe event queue.
*/
class EventQueue : public IceUtil::Shared, public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    //! Set tracing flags to TRUE to print messages to standard output.
    EventQueue( bool traceAddEvents=false, bool traceGetEvents=false );

    //! Add event to the queue.
    void add( const EventPtr & e );

    //! Get event from the queue. If the queue is empty
    //! this call blocks indefinitely until an event is added.
    void get( EventPtr & e );

    //! Get event from the queue with timeout. 
    //! Returns TRUE if event was received, FALSE if timeout occured.
    bool timedGet( EventPtr & e, int timeoutMs );

    //! Number of event in the queue
    int size() const;

private:
    std::list<EventPtr> events_;
    bool traceAddEvents_;
    bool traceGetEvents_;
};

typedef IceUtil::Handle<EventQueue> EventQueuePtr;

} // namespace

#endif
