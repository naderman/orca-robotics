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


class EventQueueOptimizer : public IceUtil::Shared
{
public:
    //! Combines this event with a new event
    //! Returns TRUE if this combination is supported, FALSE otherwise.
    //! Default implementation does not support any combinations.
    //! If combination is supported and is successful, the returned value is TRUE,
    //! all the information is transfered into the @p existing event and the 
    //! @p extra event can be safely discarded.
    virtual bool combine( EventPtr& existing, const EventPtr& extra ) { return false; };
};

typedef IceUtil::Handle<EventQueueOptimizer> EventQueueOptimizerPtr;

/*!
    Thread-safe event queue.
*/
class EventQueue : public IceUtil::Shared, public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    //! Set tracing flags to TRUE to print messages to standard output.
    EventQueue( bool traceAddEvents=false, bool traceGetEvents=false );

    void setOptimizer( EventQueueOptimizerPtr optimizer );

    //! Add event to the queue.
    void add( const EventPtr& e );

    //! Add event to the queue but, before the event is added, the queue tries to
    //! combine it with the last event already in the queue. This is done by calling 
    //! cobmine() function of the queue's optimizer. The default optimizer does not
    //! support any combinations. To implement combination(s) for your events, implement
    //! your own optimizer by deriving from EventQueueOptimizer.
    //! Calling this function when the event is empty or when the queue's optimizer is not set
    //! or when the combine() operation fails for some reason is equivalent to calling add().
    void optimizedAdd( const EventPtr& e );

    //! Get event from the queue. If the queue is empty
    //! this call blocks indefinitely until an event is added.
    void get( EventPtr& e );

    //! Get event from the queue with timeout. 
    //! Returns TRUE if event was received, FALSE if timeout occured.
    bool timedGet( EventPtr& e, int timeoutMs );

    //! Discards all events.
    void clear();

    //! Number of event in the queue
    int size() const;

private:
    std::list<EventPtr> events_;
    EventQueueOptimizerPtr optimizer_;
    bool traceAddEvents_;
    bool traceGetEvents_;
};

typedef IceUtil::Handle<EventQueue> EventQueuePtr;

//! A class which lets outsiders to post events into its own event queue.
class EventQueueHolder
{
public:
    virtual ~EventQueueHolder() {};
    //! Adds event to the queue.
    virtual void postEvent( const EventPtr& e )=0;
};

} // namespace

#endif
