/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ICEGRIDMON_MAINLOOP_H
#define ORCA2_ICEGRIDMON_MAINLOOP_H

#include <IceGrid/Registry.h>
#include <orcaice/context.h>
#include <orcaice/thread.h>

#include "registryobserverI.h"
#include "applicationobserverI.h"
#include "adapterobserverI.h"
#include "objectobserverI.h"
#include "nodeobserverI.h"

namespace icegridmon
{

class MainLoop : public orcaice::Thread
{
public:

    MainLoop( const orcaice::Context& context );

    virtual void run();

private:

    // Loops until activated
    void activate();

    bool tryCreateSession();

    IceGrid::AdminSessionPrx session_;
    IceUtil::Time timeout_;
    orcaice::Context context_;

    // Observers
    RegistryObserverI               *registryObserver_;
    IceGrid::RegistryObserverPrx     registryObserverPrx_;
    ApplicationObserverI            *applicationObserver_;
    IceGrid::ApplicationObserverPrx  applicationObserverPrx_;
    AdapterObserverI                *adapterObserver_;
    IceGrid::AdapterObserverPrx      adapterObserverPrx_;
    ObjectObserverI                 *objectObserver_;
    IceGrid::ObjectObserverPrx       objectObserverPrx_;
    NodeObserverI                   *nodeObserver_;    
    IceGrid::NodeObserverPrx         nodeObserverPrx_;
};

//typedef IceUtil::Handle<MainLoop> MainLoopPtr;

} // namespace

#endif
