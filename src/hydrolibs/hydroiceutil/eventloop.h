/*
 * Orca-Robotics Project: Components for robotics 
 *               http://!orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_EVENT_LOOP_H
#define HYDROICEUTIL_EVENT_LOOP_H

#include <gbxutilacfr/exceptions.h>
#include <hydroiceutil/eventjob.h>
#include <map>

namespace hydroiceutil {

//! This exception is raised when something is wrong with the hardware.
class ReceiverIdNotExistException : public gbxutilacfr::Exception
{
public:
    ReceiverIdNotExistException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
    ReceiverIdNotExistException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};


/*! 
This is a convenience class which makes the JobQueue appear like an event loop.
Uses specialized EventJob jobs to dispatch events through the JobQueue.
 
This class queues events and executes them. For a class which does not execute the events, see EventQueue. 

Typically, an event loop has a single execution thread to ensure that the events are delivered in the order
in which they were added. This class does not enforce this convention and leaves it up to the user to decide
(the default size of the thread pool in JobQueue::Config is 1).

EventLoop offers a facility to store event receiver ID's which can be used later to dispatch
events. All public methods utilizing the receiver registry are thread safe.
*/
class EventLoop : public JobQueue
{
public:
    //! Constructor
    EventLoop( gbxutilacfr::Tracer& tracer, const JobQueue::Config& config=JobQueue::Config() );

    //! Adds the event @c event, with the @c receiver as the receiver of the event, to an event queue and returns immediately. 
    //! It is not safe to modify or delete the event after it has been posted.
    void postEvent( EventReceiver& receiver, const EventPtr &event, const std::string& signature="", bool debug=false );

    //! Executes an EventJob which sends an Event of type @c type to the event receiver @c receiver.
    void postEvent( EventReceiver& receiver, int type, const std::string& signature="", bool debug=false );

    //! Executes an EventJob which sends an event @c event to the event receiver with ID @c receiverID.
    //! Throws ReceiverIdNotExistException if no receiver with specified ID is registered.
    void postEvent( const std::string& receiverId, const EventPtr &event, const std::string& senderId="", bool debug=false );

    //! Executes an EventJob which sends an Event of type @c type to the event receiver with ID @c receiverID.
    //! Throws ReceiverIdNotExistException if no receiver with specified ID is registered.
    void postEvent( const std::string& receiverId, int type, const std::string& senderId="", bool debug=false );

    //! Posts event @c event to all registered receivers.
    void postEventToAll( const EventPtr &event, const std::string& senderId="", bool debug=false );

    //! Posts event of type @c type to all registered receivers.
    void postEventToAll( int type, const std::string& senderId="", bool debug=false );

    //! Registers receiver with specified ID. ID can be any string.
    //! If a receiver with the specified ID is already registered, it's overwritten.
    //! If you want to check for uniqueness first, use @c receiver() function.
    //! It is OK to register the same receiver multiple times under different names
    //! (e.g. to create a well known alias).
    //! Throws gbxutilacfr::Exception if @c receiver is a null pointer.
    void addReceiver( const std::string& receiverId, EventReceiver* receiver );

    //! Unregisters receiver with specified ID.
    //! If a receiver with the specified ID is not registered the request is quietly ignored.
    void removeReceiver( const std::string& receiverId );

    //! Returns null pointer if the specified ID is not registered.
    EventReceiver* receiver( const std::string& receiverId );

private:
    
    std::map<std::string,EventReceiver*> registry_;
    IceUtil::Mutex registryMutex_;
    
};
//! Ice smart pointer to EventLoop
typedef IceUtil::Handle<EventLoop> EventLoopPtr;

} // namespace

#endif
