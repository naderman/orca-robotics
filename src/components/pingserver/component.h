/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef PINGSERVER_COMPONENT_H
#define PINGSERVER_COMPONENT_H

#include <orcaice/component.h>
#include <orca/tracer.h>
#include "pingerI.h"

namespace pingserver {

//
// @author Alex Brooks
//
class Component : public orcaice::Component
{

public: 

    Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private: 

    PingerI *pingerInterface_;
};

}

#endif
