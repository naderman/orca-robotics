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
@brief A class implementing a common subsystem state machine.

The (simple) state machine implemented by this class is a chain of state transitions 
(with one extra link):

Idle --> Initialising --> Working --> Finalising --> Shutdown
              |___________________________^

The following represents the Subsystem state machine in the format of
State Machine Compiler (see smc.sf.net) :
@verbatim
Idle
Entry { init(); }
{
    init
    Initialising
    {}
}

Initialising
Entry { initialise(); }
{
    [ !isStopping ] finished
    Working
    {}

    [ isStopping ] finished
    Finalising
    {}
}

Working
Entry { work(); }
{
    finished
    Finalising
    {}
}

Finalising
Entry { finalise(); }
{
    finished
    Shutdown
    {}
}

Shutdown
{
}   
@endverbatim
 */
class Subsystem : public orcaice::SubsystemThread
{
public: 
    //! Constructor.
    Subsystem( const orcaice::Context &context, const std::string& subsysName="MainSubsystem" );
    virtual ~Subsystem() {};

protected:
    // implementation note: FSM actions need to be protected so that the derived class
    // can call them without re-implementing.

    //! Action performed when in Intialising state.
    //! Default imlementation does nothing.
    virtual void initialise() {};
    
    //! Action performed when in Working state.
    //! Default imlementation does nothing.
    virtual void work() {};

    //! Action performed when in Finalising state.
    //! Default imlementation does nothing.
    virtual void finalise() {};

    //! Component context.
    orcaice::Context context_;

private:
    // from SubsystemThread
    virtual void walk();

};
//! A smart pointer to the Subsystem class.
typedef IceUtil::Handle<Subsystem> SubsystemPtr;

}

#endif
