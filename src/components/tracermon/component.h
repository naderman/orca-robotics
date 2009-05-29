/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>

namespace tracermon
{

class Component : public orcaice::Component
{
public:
    Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:
    // NOTE: fragile design, the order of member declaration is very important.
    // usrThread_ is created first so that it's destructed last.
    gbxiceutilacfr::ThreadPtr usrThread_;
    gbxiceutilacfr::ThreadPtr netThread_;
};

} // namespace

#endif
