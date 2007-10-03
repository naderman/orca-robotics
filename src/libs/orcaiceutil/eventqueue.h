/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_EVENT_QUEUE_H
#define ORCAICEUTIL_EVENT_QUEUE_H

#include <list>

#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>

namespace orcaiceutil
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
    @brief Thread-safe event queue.

    First define custom events.
@verbatim
#include <orcaiceutil/eventqueue.h>

enum EventType { Activate };

class ActivateEvent : public orcaiceutil::Event
{
public:
    ActivateEvent() : orcaiceutil::Event( Activate ) {};
    bool isUrgent;
};
typedef IceUtil::Handle<ActivateEvent> ActivateEventPtr;
@endverbatim

Your threaded class will have an event queue, probably as a member variable.
@verbatim
orcaiceutil::EventQueuePtr events_;
@endverbatim

Don't forget to initialize it in the constructor

@verbatim
events_(new orcaiceutil::EventQueue)
@endverbatim

When running inside your thread, read from the queue, with or without timeout. 
Here's an example of doing it inside the run function of orcaiceutil::Thread.

@verbatim
void run()
{
    orcaiceutil::EventPtr event;
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

typedef IceUtil::Handle<EventQueue> EventQueuePtr;

/*! 
    @brief A class which lets outsiders to post events into its own event queue.
*/
class EventQueueHolder
{
public:
    virtual ~EventQueueHolder() {};
    /*! 
        Adds event to the queue. A typical implementation, if the derived class 
        has an event queue as a member:
@verbatim
virtual void postEvent( const orcaiceutil::EventPtr& e ) { events_->add( e ); };
@endverbatim
    */
    virtual void postEvent( const EventPtr& e )=0;
};

} // namespace

#endif
