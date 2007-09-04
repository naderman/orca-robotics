/*
Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
This copy of Orca is licensed to you under the terms described in the
ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICE_SAFE_THREAD_H
#define ORCA2_ORCAICE_SAFE_THREAD_H

#include <orcaice/thread.h>

namespace orcaice
{

/*!
@brief A version of the Thread class which catches all possible exceptions.

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

@see Thread.
 */
class SafeThread : public Thread
{
public:
    // from IceUtil::Thread
    //! This implementation calls walk(), catches all possible exceptions, prints out 
    //! errors and waits for someone to call stop().
    virtual void run();

    //! Implement this function in the derived class and put here all the stuff which your
    //! thread needs to do.
    virtual void walk()=0;

};

} // end namespace

#endif
