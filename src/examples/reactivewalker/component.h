/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_REACTIVE_WALKER_COMPONENT_H
#define ORCA2_REACTIVE_WALKER_COMPONENT_H

#include <orcaice/component.h>
#include <hydroutil/thread.h>

namespace reactivewalker
{

class AlgoHandler;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    hydroutil::ThreadPtr algoThread_;

};

} // namespace

#endif
