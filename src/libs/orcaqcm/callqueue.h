/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QORCACM_CALL_QUEUE_H
#define QORCACM_CALL_QUEUE_H

#include <list>

#include <IceUtil/Thread.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Shared.h>
#include <Ice/Exception.h>

namespace orcaqcm
{

class Call : public IceUtil::Shared
{
public:

    virtual void execute() = 0;
};

typedef IceUtil::Handle<Call> CallPtr;

class CallQueue : public IceUtil::Thread, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    CallQueue();
    ~CallQueue() {};

    void add( const CallPtr & );
    void stop();
    virtual void run();

protected:
    // Check from inside the thread if it's time to exit.
    bool isActive();

private:

    std::list<CallPtr> requests_;
    bool destroy_;
    std::auto_ptr<Ice::Exception> exception_;
};

typedef IceUtil::Handle<CallQueue> CallQueuePtr;

} // namespace

#endif
