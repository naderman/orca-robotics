/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "eventscheduler.h"
#include "timeutils.h"
#include <IceUtil/Time.h>
#include <gbxutilacfr/exceptions.h>
#include <iostream>

using namespace std;
using namespace hydroiceutil;

void 
EventScheduler::Config::validate( gbxutilacfr::Tracer& tracer )
{
    // nothing to validate yet
}

//////////////////////////////////////////////

EventScheduler::EventScheduler( gbxutilacfr::Tracer& tracer, const Config& config ) :
    tracer_(tracer),
    config_(config)
{
    config_.validate( tracer_ );

    // auto-start
    this->start();
}

void
EventScheduler::scheduleEvent( EventReceiver& receiver, const EventPtr &event, double delay )
{
    EventItem item( receiver, event );
    // calculate absolute dispatch time
    IceUtil::Time dispatchTime = IceUtil::Time::now() + hydroiceutil::timeFromDouble( delay );

    IceUtil::Mutex::Lock lock(scheduleMutex_);
    schedule_.insert( std::make_pair( dispatchTime,item) );

    if ( config_.traceScheduledEvents ) {
        stringstream ss;
        ss<<"EventScheduler: Added "<<event->type()<<" to the schedule.";
        tracer_.info( ss.str() );
    }
}

void
EventScheduler::scheduleEvent( EventReceiver& receiver, int type, double delay )
{
    scheduleEvent( receiver, new hydroiceutil::Event(type), delay );
}

void
EventScheduler::dispatchEvents()
{
    while ( !isStopping() )
    {
        bool isEventRipe = false;
        EventReceiver* nextReceiver;
        EventPtr nextEvent;

        // minimize critical section, so that we can schedule events from inside the event
        {
            IceUtil::Mutex::Lock lock(scheduleMutex_);

            // can't put it into while() statement without expanding the critical section
            if ( schedule_.empty() )
                break;

            // this is a copy (we use it inside the critical section)
            const IceUtil::Time nextTime = schedule_.begin()->first;
            // this is a dumb pointer
            nextReceiver = &schedule_.begin()->second.receiver;
            // this is smart pointer
            nextEvent = schedule_.begin()->second.event;

            // check now if this event should be sent
            if ( nextTime < IceUtil::Time::now() ) {
                isEventRipe = true;

                // delete it before actually executing
                schedule_.erase( schedule_.begin() );
            }
        }
        // end of critical section, now we can do what we want

        if ( isEventRipe ) 
        {
            if ( config_.traceDispatchedEvents ) {
                stringstream ss;
                ss<<"EventScheduler: Dispatching "<<nextEvent->type()<<" to the receiver.";
                tracer_.info( ss.str() );
            }

            // dispatch
            nextReceiver->event( nextEvent );
        } 
        else 
        {
            // the next scheduled event is still in the future
//             cout<<"DEBUG: no more ripe events left"<<endl;
            break;
        }   
    }
}

void 
EventScheduler::run()
{
    // make a copy of this parameter so we don't have lock everything just for it
    int pollIntervalMs = config_.pollIntervalMs;

    while ( !isStopping() )
    {
        dispatchEvents();

        if ( !isStopping() )
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(pollIntervalMs));
    }
}
