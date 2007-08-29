/*
Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
This copy of Orca is licensed to you under the terms described in the
ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICE_THREAD_H
#define ORCA2_ORCAICE_THREAD_H

#include <IceUtil/Thread.h>

namespace orcaice
{

/*!
@brief A minor extention of the Ice::Thread class.

Adds an option to stop a thread with @ref stop(). Requires that the thread periodically checks
whether it has to stop by calling isStopping(); Since @ref stop() is public, it can be called
from inside or outside of the derived class.

To use this class, simply overload the virtual Ice::Thread::run() function.
@verbatim
void MyThread::run()
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

The implicit state machine of this class has 4 states {Starting, Running, Stopping, Stopped}. Events and the corresponding state transitions are listed below. Note that the final state may only be accessed when using smart pointer orcaice::ThreadPtr (otherwise the thread self-destructs).

@verbatim
EVENT      : Constructor Ice::Thread::Thread()
TRANSITION : Starting    
INTERNAL   : isStopping()=false, isAlive()=false, isStarted()=false

EVENT      : Ice::Thread::start()
TRANSITION : Starting -> Running     
INTERNAL   : isStopping()=false, isAlive()=true, isStarted()=true

EVENT      : orcaice::Thread::stop()
TRANSITION : Running -> Stopping    
INTERNAL   : isStopping()=true, isAlive()=true, isStarted()=true

EVENT      : termination of run() function.
TRANSITION : Stopping -> Stopped     
INTERNAL   : isStopping()=true, isAlive()=false, isStarted()=true
@endverbatim

Caveats: 
- Make sure you catch all exception which can possibly be raised inside Ice::Thread::run.
Otherwise, you'll see "uncaught exception" printed out and the component will hang.
- orcaice::Threads self-destruct (ie call their own destructor) when orcaice::Thread::run returns, unless you hold onto an orcaice::ThreadPtr which points to it. So never call @c delete on the pointer to a thread, doing so will result in segmentation fault.
 */
class Thread : public IceUtil::Thread
{
public:

    Thread();

    //! Lets the thread know that it's time to stop. Thread-safe, so it can be called
    //! from inside or outside this thread.
    void stop();

    //! Returns TRUE if the thread is in Started state, FALSE otherwise.
    bool isStarted();

    //! Returns TRUE if the thread is in Stopping state, FALSE otherwise.
    bool isStopping();

    //! @b Depricated function! Use isStopping() instead (note that it returns the opposite).
    //! 
    //! Returns FALSE if thread was told to stop, TRUE otherwise.
    bool isActive() { return !isStopping(); };

protected:

    //! Wait for someone from the outside to call @ref stop.
    //! It may be necessary to call this function before exitting from Ice::Thread::run after
    //! catching an exception. If we just exit from run() and someone calls our @ref stop()
    //!  function afterwards there's a possibility of lock up.
    void waitForStop();

private:
    bool isStopping_;
};
//! A smart pointer to the thread class.
typedef IceUtil::Handle<Thread> ThreadPtr;

//! A convenience function which first stops the @p thread and then waits for it to
//! terminate. If the pointer is NULL, this function quietly returns.
void stopAndJoin( orcaice::Thread* thread );

//! A convenience function which first stops the @p thread and then waits for it to
//! terminate. If the smart pointer is 0, this function quietly returns.
void stopAndJoin( orcaice::ThreadPtr thread );

} // end namespace

#endif
