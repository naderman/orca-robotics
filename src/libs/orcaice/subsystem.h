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

namespace orcaice {

/*!
@brief A class implementing the common subsystem state machine.

The state machine is defined by gbxutilacfr::Status.
  
@endverbatim
 */
class Subsystem : public orcaice::SubsystemThread
{
public: 
    //! Constructor.
    Subsystem( gbxutilacfr::Tracer& tracer, gbxutilacfr::Status& status, const std::string& subsysName="SubsystemThread" );
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

private:
    // from SubsystemThread
    virtual void walk();

};
//! A smart pointer to the Subsystem class.
typedef IceUtil::Handle<Subsystem> SubsystemPtr;

}

#endif
