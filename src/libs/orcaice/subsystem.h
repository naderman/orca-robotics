/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_SUBSYSTEM_H
#define ORCAICE_SUBSYSTEM_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

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
class Subsystem : public orcaice::SubsystemThread
{
public: 
    Subsystem( const orcaice::Context &context, const std::string& subsysName="MainSubsystem" );
    virtual ~Subsystem() {};

    // FSM actions
    virtual void initialise();
    virtual void work();
    virtual void finalise();

    // from SubsystemThread
    virtual void walk();

protected:
    orcaice::Context context_;

};
//! A smart pointer to the Subsystem class.
typedef IceUtil::Handle<Subsystem> SubsystemPtr;

}

#endif
