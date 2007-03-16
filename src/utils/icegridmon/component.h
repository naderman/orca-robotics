/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ICEGRIDMON_COMPONENT_H
#define ORCA2_ICEGRIDMON_COMPONENT_H

#include <vector>
#include <string>
#include <orcaice/component.h>
#include <IceGrid/Session.h>
#include "sessionkeepalive.h"
#include "registryobserverI.h"
#include "applicationobserverI.h"
#include "adapterobserverI.h"
#include "objectobserverI.h"
#include "nodeobserverI.h"

namespace icegridmon
{

class Component : public orcaice::Component
{
public:

    Component( const std::string & compName );
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    SessionKeepAliveThreadPtr sessionKeepAlive_;
    IceGrid::AdminSessionPrx  session_;

    RegistryObserverI    *registryObserver_;
    ApplicationObserverI *applicationObserver_;
    AdapterObserverI     *adapterObserver_;
    ObjectObserverI      *objectObserver_;
    NodeObserverI        *nodeObserver_;
};

}

#endif
