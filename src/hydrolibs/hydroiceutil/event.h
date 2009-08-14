/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_EVENT_H
#define HYDROICEUTIL_EVENT_H

#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>

namespace hydroiceutil
{

//! Generic event which can be sent directly to a EventReceiver or added to the EventQueue.
//!
//! @see EventReceiver, EventQueue
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
//! Smart pointer to Event
typedef IceUtil::Handle<Event> EventPtr;


/*! 
@brief A class which lets outsiders to post events into its own event queue.

An outsider is only allowed to post events, but not to get or clear them, etc.
*/
class EventReceiver
{
public:
    virtual ~EventReceiver() {};

    //! This virtual function receives events sent to an object and should return true if 
    //! the event @c e was recognized and processed.
    virtual bool event( const EventPtr& e )=0;

    //! Convenience function, creates an Event of given type and calls the function above.
    bool event( int type ) { return event( new hydroiceutil::Event(type) ); };
};

} // namespace

#endif
