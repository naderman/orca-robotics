/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <IceUtil/Time.h>

#include "thread.h"

using namespace orcaice;

Thread::Thread() : 
    isActive_(true) 
{
}

void Thread::stop()
{
    IceUtil::Mutex::Lock lock(mutex_);
    isActive_ = false;
}

void Thread::waitForStop()
{
    while ( isActive() ) {
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(10));
    }
}

bool Thread::isActive()
{
    IceUtil::Mutex::Lock lock(mutex_);
    return isActive_;
}

void orcaice::stopAndJoin( orcaice::Thread* thread )
{
    if ( thread ) {
        // get the control object first
        IceUtil::ThreadControl tc = thread->getThreadControl();
        
        // Tell the thread to stop
        thread->stop();
    
        // Then wait for it
        tc.join();
    }
}

void orcaice::stopAndJoin( orcaice::ThreadPtr thread )
{
    if ( thread ) {
        // get the control object first
        IceUtil::ThreadControl tc = thread->getThreadControl();
        
        // Tell the thread to stop
        thread->stop();
    
        // Then wait for it
        tc.join();        
    }
}
