/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_SUBSTATUS_THREAD_H
#define ORCAICE_SUBSTATUS_THREAD_H

#include <gbxsickacfr/gbxiceutilacfr/thread.h>
#include <gbxutilacfr/substatus.h>
#include <gbxutilacfr/status.h>
#include <gbxutilacfr/tracer.h>

namespace orcaice {

/*!
@brief A version of the Thread class which catches all possible exceptions and integrates some Status operations.

If an exception is caught when the thread is not stopping, a status fault is issued.
Then the thread will wait for someone to call stop().

To use this class, simply implement the pure virtual walk() function.
@verbatim
void MyThread::walk()
{
    // initialize

    // main loop
    while ( !isStopping() )
    {
        // do something
    }

    // clean up
}
@endverbatim

 */
class SubstatusThread : public gbxiceutilacfr::Thread
{
public:
    //! Supply an optional Tracer and Status. The optional @c subsysName is used in reporting status changes
    //! as the subsystem name.
    SubstatusThread( gbxutilacfr::Tracer& tracer, gbxutilacfr::Status& status, const std::string& subsysName="SubstatusThread" );

    // from IceUtil::Thread
    //! This implementation calls walk(), catches all possible exceptions, prints out 
    //! errors and waits for someone to call stop().
    virtual void run();

    //! Access to subsystem status.
    gbxutilacfr::SubStatus& subStatus() { return subStatus_; };

    //! Returns subsystem name assigned to this thread.
    std::string subsysName() const { return subStatus_.name(); };

private:

    //! Implement this function in the derived class and put here all the stuff which your
    //! thread needs to do.
    virtual void walk()=0;

    gbxutilacfr::Tracer& tracer_;
    gbxutilacfr::SubStatus subStatus_;
};
//! A smart pointer to the SubstatusThread class.
typedef IceUtil::Handle<SubstatusThread> SubstatusThreadPtr;

} // end namespace

#endif
