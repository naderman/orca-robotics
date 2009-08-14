/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_EVENT_QUEUE_H
#define HYDROICEUTIL_EVENT_QUEUE_H

#include <list>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>
#include <hydroiceutil/event.h>

namespace hydroiceutil
{

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

//! Smart pointer to EventQueueOptimizer
typedef IceUtil::Handle<EventQueueOptimizer> EventQueueOptimizerPtr;

/*!
@brief Thread-safe event queue.

This class does not execute the events. For an event execution loop see EventLoop.

First define some custom events.
@verbatim
#include <hydroiceutil/eventqueue.h>

enum EventType { Activate };

class ActivateEvent : public hydroiceutil::Event
{
public:
    ActivateEvent( bool urgent=false ) : hydroiceutil::Event( Activate ), isUrgent(urgent) {};
    bool isUrgent;
};
typedef IceUtil::Handle<ActivateEvent> ActivateEventPtr;
@endverbatim

Your threaded class will have an event queue, probably as a member variable.
@verbatim
hydroiceutil::EventQueuePtr events_;
@endverbatim

Don't forget to initialize it in the constructor

@verbatim
events_(new hydroiceutil::EventQueue)
@endverbatim

When running inside your thread, read from the queue, with or without timeout. 
Here's an example of doing it inside the run function of gbxiceutilacfr::Thread.

@verbatim
void run()
{
    hydroiceutil::EventPtr event;
    int timeoutMs = 500;
    
    while ( !isStopping() )
    {
        if ( !events_->timedGet( event, timeoutMs ) ) {
            // timed out
            continue;
        }
    
        switch ( event->type() )
        {
            // approx in order of call frequency
            case Activate : {
                ActivateEventPtr e = ActivateEventPtr::dynamicCast( event );
                if ( e->isUrgent )
                    // do something
                else
                    // don't do anything
                break;
            }
            default : {
                cout<<"Unknown event "<<event->type()<<". Ignoring..."<<endl;
            }
        } // switch
    } // while
}
@endverbatim

Asynchronously from your main thread loop, you can add events to your queue.

@verbatim
void outsideFunctionCall()
{
    events_->add( new ActivateEvent() );
}
@endverbatim

@see EventQueueHolder
*/
class EventQueue : public IceUtil::Shared, public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    //! Set tracing flags to TRUE to print messages to standard output.
    EventQueue( bool traceAddEvents=false, bool traceGetEvents=false );

    //! Set the optimizer.
    void setOptimizer( EventQueueOptimizerPtr optimizer );

    //! Add event to the queue.
    void add( const EventPtr& e );

    //! Add event of type @c type to the queue.
    void add( int type );

    //! Add event to the queue but, before the event is added, the queue tries to
    //! combine it with the last event already in the queue. This is done by calling 
    //! combine() function of the queue's optimizer. The default optimizer does not
    //! support any combinations. To implement combination(s) for your events, write
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
//! Smart pointer to EventQueue
typedef IceUtil::Handle<EventQueue> EventQueuePtr;

/*! 
@brief A class which lets outsiders to post events into its own event queue.

An outsider is only allowed to post events, but not to get or clear them, etc.
*/
class EventQueueHolder : public IceUtil::Shared
{
public:
    virtual ~EventQueueHolder() {};
    /*! 
        Adds event to the queue. A typical implementation, if the derived class 
        has an event queue as a member:
@verbatim
virtual void postEvent( const hydroiceutil::EventPtr& e ) { events_->add( e ); };
@endverbatim
    */
    virtual void postEvent( const EventPtr& e )=0;

    //! Convenience function, posts an Event of given type.
    // implementation note:
    // I'd prefer to make this function polymorphic, postEvent(int), but ...
    // it will be hidden by the derived class's implementation of postEvent(const EventPtr&)
    // see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
    // It could be "unhidden" with 'using' in the derived class, but this is super ugly.
    //      using hydroiceutil::EventQueueHolder::postEvent;
    virtual void postEventWithType( int type ) { postEvent( new hydroiceutil::Event(type) ); };
};
//! Smart pointer to EventQueueHolder
typedef IceUtil::Handle<EventQueueHolder> EventQueueHolderPtr;

} // namespace

#endif
