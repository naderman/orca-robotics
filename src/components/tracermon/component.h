/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_COMPONENT_H
#define ORCA2_TRACERMON_COMPONENT_H

#include <orcaice/component.h>
#include <hydroutil/thread.h>

namespace tracermon
{

class NetworkHandler;
class UserHandler;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    hydroutil::Thread* netHandler_;
    hydroutil::Thread* usrHandler_;
};

} // namespace

#endif
