/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <hydroutil/thread.h>

namespace brick
{

class Component: public orcaice::Component
{

public:
    Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:

    hydroutil::ThreadPtr mainThread_;
};

} // namespace

#endif
