/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ICEGRIDMON_COMPONENT_H
#define ORCA2_ICEGRIDMON_COMPONENT_H

#include <vector>
#include <string>
#include <orcaice/component.h>
#include <IceGrid/Session.h>
#include <orcaicegrid/sessionmanager.h>
#include "sessioncreationcallback.h"

namespace icegridmon
{

class SessionManagerStarter;

class Component : public orcaice::Component
{
public:

    Component( const std::string & compName );
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    SessionCreationCallback     *sessionCreationCallback_;
    orcaicegrid::SessionManager *sessionManager_;
    SessionManagerStarter       *sessionManagerStarter_;
};

}

#endif
