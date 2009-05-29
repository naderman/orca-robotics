/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_THREAD_H
#define ORCAICE_COMPONENT_THREAD_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>
#include <orcaice/component.h> // for ComponentAdapterActivationPolicy

namespace orcaice {
namespace detail {

//
// This thread is started by the component, and is used:
// - to repeatedly try to register the Home interface, if the first attempt failed.
// - to check on component Status.
//
// @author Alex Brooks
//
class ComponentThread : public gbxiceutilacfr::SafeThread
{

public: 

    ComponentThread( ComponentAdapterActivationPolicy adapterPolicy,
                     const orcaice::Context& context );

private: 

    virtual void walk();

    // Returns: true = success
    bool tryRegisterHome();

    ComponentAdapterActivationPolicy adapterPolicy_;
    orcaice::Context context_;
};

}
}

#endif
