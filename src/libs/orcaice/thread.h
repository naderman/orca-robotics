/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICE_THREAD_H
#define ORCA2_ORCAICE_THREAD_H

#include <IceUtil/Thread.h>
#include <IceUtil/Mutex.h>

namespace orcaice
{

/*!
 *  @brief A minor extention of the Ice Thread class.
 * 
 *  Adds an option to stop a thread with @ref stop(). Requires that the thread periodically checks
 *  whether it has to stop by calling isActive(); Since @ref stop() is public, it can be called
 *  from inside or outside of the derived class.
 *
 *  To use, simply overload the virtual Ice::Thread::run() function.
@verbatim
void MyThread::run()
{
    // initialize

    // main loop
    while ( isActive() )
    {
        // do something
    }

    // clean up
}
@endverbatim
 *  Caution: Make sure you catch all exception which can possibly be raised inside Ice::Thread::run.
 *  Otherwise, you'll see "uncaught exception" printed out and the component will hang.
 *
 *  Caution: Ice threads self-destruct (ie call their own destructor) when Ice::Thread::run returns.
 *  So never call @c delete on the pointer to a thread, doing so will result in segmentation fault.
 */
class Thread : public IceUtil::Thread
{
public:

    Thread();

    //! Lets the tread know that it's time to exit. Thread-safe, so it can be called
    //! from inside or outside this thread.
    void stop();

    //! A convenience function which first stops the @p thread and then waits for it to
    //! terminate. If the pointer is NULL, this function quietly returns.
    static void stopAndJoin( orcaice::Thread* thread );

protected:

    //! Check from inside the thread if it's time to exit.
    bool isActive();

    //! Wait for someone from the outside to call @ref stop.
    //! It may be necessary to call this function before exitting from Ice::Thread::run after
    //! catching an exception. If we just exit run and someone calls our @ref stop() function
    //! afterwards there's a possibility of lock up.
    void waitForStop();

private:
    bool isActive_;
    IceUtil::Mutex mutex_;

};

} // end namespace

#endif
