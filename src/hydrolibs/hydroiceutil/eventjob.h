/*
 * Orca-Robotics Project: Components for robotics 
 *               http://!orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_EVENT_JOB_H
#define HYDROICEUTIL_EVENT_JOB_H

#include <hydroiceutil/event.h>
#include <hydroiceutil/jobqueue.h>

namespace hydroiceutil {

//!
//! A special job which simply sends an event to the specified event receiver.
//! By convention, the event receiver does not take much time processing the event.
//!
class EventJob : public Job
{
public:
    //! Constructor with an event. Optional signature may contain the sender and receiver
    //! of the event. If supplied, this information will be displayed when toString()
    //! is called. This is very useful for debuging, e.g. when the event is not excepted.
    EventJob( EventReceiver& receiver, const EventPtr &event, const std::string& signature="" );

    //! Constructor with an event type
    EventJob( EventReceiver& receiver, int type );

    //! Sends event of specified type to the receiver.
    //! Throws gbxutilacfr::Exception if the receiver does not accept the event.
    void execute();

    //! Returns the name of the job with the event type.
    virtual std::string toString() const;

    //! Dispatching an event should be very quick.
    virtual int expectedDuration() const { return 2; };

    //! EXPERIMENTAL! Note that access to this variable is not thread-safe.
    //! Issue warning if event processing takes longer than this many microseconds.
    //! Duration is not measured and no warnings are issued if this value is negative (default).
    static int _warnDurationUs;

private:

    EventReceiver& receiver_;
    EventPtr       event_;
    std::string signature_;
};
//! Ice smart pointer to the EventJob.
typedef IceUtil::Handle<EventJob> EventJobPtr;

} // namespace

#endif
