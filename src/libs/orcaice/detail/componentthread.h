/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_THREAD_H
#define ORCAICE_COMPONENT_THREAD_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <gbxutilacfr/status.h>
#include <orcaice/context.h>
#include <orcaice/component.h>

namespace orcaice {

//
// This thread is started by the component, and used to:
// - repeatedly try to register the Home interface, if the first one failed.
// - check on component Status.
//
// @author Alex Brooks
//
class ComponentThread : public gbxiceutilacfr::SafeThread
{

public: 

    ComponentThread( const Ice::ObjectPrx   &homePrx,
                     gbxutilacfr::Status    &status,
                     ComponentInterfaceFlag  interfaceFlag,
                     const orcaice::Context &context );
    ~ComponentThread();

    virtual void walk();

private: 

    // Returns: true = success
    bool tryRegisterHome();

    bool registeredHome_;

    Ice::ObjectPrx          homePrx_;
    gbxutilacfr::Status    &status_;
    ComponentInterfaceFlag  interfaceFlag_;
    orcaice::Context        context_;
};

}

#endif
