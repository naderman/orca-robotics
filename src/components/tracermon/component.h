/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_COMPONENT_H
#define ORCA2_TRACERMON_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/thread.h>

namespace tracermon
{

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    orcaice::Thread* netHandler_;
    orcaice::Thread* usrHandler_;
};

} // namespace

#endif
