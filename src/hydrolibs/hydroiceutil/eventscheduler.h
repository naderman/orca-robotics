/*
 * Orca-Robotics Project: Components for robotics 
 *               http://!orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_EVENT_SCHEDULER_H
#define HYDROICEUTIL_EVENT_SCHEDULER_H

#include <gbxutilacfr/tracer.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>
#include <hydroiceutil/event.h>
#include <IceUtil/Time.h>
#include <IceUtil/Mutex.h>
#include <map>

namespace hydroiceutil {

//! A simple event schedule. Only very inaccurate dispatch can be expected. The scheduler
//! sleeps for a while, wakes up, checks for events which are past-due and dispatches all
//! of them in order. The resolution of the scheduler can be controlled somewhat by
//! changing the interval between polls.
//!
//! Scheduling events from inside the dispatched event is allowed.
class EventScheduler : public gbxiceutilacfr::Thread
{
public:
    //! Event schedule configuration structure
    struct Config
    {
        Config() :
            pollIntervalMs(500),
//             lateDispatchToleranceMs(-1),
            traceScheduledEvents(false),
            traceDispatchedEvents(false) {};

        //! Throws Exception when mis-configured.
        void validate( gbxutilacfr::Tracer& tracer );

        //! Controls the interval between polls for events ready to be dispatched.
        //! Default if 500ms.
        int pollIntervalMs;
        // Issue warning when a scheduled event is dispatched more then @c lateDispatchToleranceMs [ms]
        // late. Default is -1 (no warnings);
//         int lateDispatchToleranceMs;
        //! Use for debugging. If TRUE, scheduling of each event is traced to info().
        //! Default is FALSE.
        bool traceScheduledEvents;
        //! Use for debugging. If TRUE, dispatching of each event is traced to info().
        //! Default is FALSE.
        bool traceDispatchedEvents;
    };

    //! Constructor. 
    EventScheduler( gbxutilacfr::Tracer& tracer, const EventScheduler::Config& config=EventScheduler::Config() );

    //! Will send event @c event to the event reciever @c receiver after @c delay [seconds].
    void scheduleEvent( EventReceiver& receiver, const EventPtr &event, double delay );

    //! Will send an hydroiceutil::Event event of type @c type to the event reciever @c receiver 
    //! after @c delay [seconds].
    void scheduleEvent( EventReceiver& receiver, int type, double delay );

    // from Thread
    virtual void run();

private:
    // storing absolute time of the scheduled events (so that we don't have to recompute them every time).
    // Allow duplicates with multimap in case 2 events happen to be scheduled for the same instant.
    struct EventItem
    {
        EventItem( EventReceiver& r, const EventPtr& e ) :
            receiver(r), event(e) {};
        EventReceiver& receiver;
        EventPtr event;
    };
    typedef std::multimap<IceUtil::Time,EventItem> TimeEventMap;
    TimeEventMap schedule_;
    IceUtil::Mutex scheduleMutex_;

    void dispatchEvents();
    
    gbxutilacfr::Tracer& tracer_;
    Config config_;
};
//! Ice smart pointer to EventScheduler
typedef IceUtil::Handle<EventScheduler> EventSchedulerPtr;

} // namespace

#endif
